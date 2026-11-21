#include "app_ble_stream.h"

#include "app_scheduler.h"
#include "app_timer.h"
#include "ble_main.h"
#include "i18n/language.h"
#include "mini_app_launcher.h"
#include "mini_app_registry.h"
#include "mui_include.h"
#include "nrf_pwr_mgmt.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define STREAM_FRAME_BYTES 1024 // 128 * 64 / 8
#define STREAM_MAGIC_0     0xA5
#define STREAM_MAGIC_1     0x5A
#define STREAM_FLAG_RAW       0x00
#define STREAM_FLAG_RLE       0x01
#define STREAM_FLAG_DELTA_RLE 0x02

typedef enum {
    STREAM_VIEW_ID_MAIN = 0,
} stream_view_id_t;

typedef enum {
    RX_WAIT_MAGIC0 = 0,
    RX_WAIT_MAGIC1,
    RX_WAIT_FLAGS,
    RX_WAIT_LEN_LO,
    RX_WAIT_LEN_HI,
    RX_WAIT_PAYLOAD,
} stream_rx_state_t;

typedef struct {
    mui_view_dispatcher_t *p_view_dispatcher;
    mui_view_t *p_view;
    char ble_addr[20];
    bool connected;
    bool has_frame;
    bool show_hud;
    bool invert;
    volatile bool redraw_pending;
    uint32_t frame_count;
    uint32_t fps_window_ticks;
    uint16_t fps_window_frames;
    uint16_t cur_fps;
    uint16_t last_pkt_len;
    int8_t last_rssi;
    bool has_rssi;

    stream_rx_state_t rx_state;
    uint8_t rx_flags;
    uint16_t rx_expected_len;
    uint16_t rx_pos;
    uint8_t rx_buf[STREAM_FRAME_BYTES];
    uint8_t frame_buf[STREAM_FRAME_BYTES];
} app_ble_stream_t;

static app_ble_stream_t *s_stream_app = NULL;

static const char *stream_tr(const char *zh, const char *en) {
    Language lang = getLanguage();
    return (lang == LANGUAGE_ZH_HANS || lang == LANGUAGE_ZH_TW) ? zh : en;
}

static void stream_redraw_sched_handler(void *p_event_data, uint16_t event_size) {
    (void)p_event_data;
    (void)event_size;
    if (s_stream_app != NULL) {
        s_stream_app->redraw_pending = false;
        mui_update(mui());
    }
}

static void stream_request_redraw(app_ble_stream_t *app) {
    if (app != NULL && !app->redraw_pending) {
        app->redraw_pending = true;
        if (app_sched_event_put(NULL, 0, stream_redraw_sched_handler) != NRF_SUCCESS) {
            app->redraw_pending = false;
        }
    }
}

static void stream_conn_sched_handler(void *p_event_data, uint16_t event_size) {
    (void)event_size;
    if (s_stream_app == NULL || p_event_data == NULL) {
        return;
    }
    bool connected = (*(uint8_t *)p_event_data) != 0;
    s_stream_app->connected = connected;
    s_stream_app->rx_state = RX_WAIT_MAGIC0;
    if (!connected) {
        s_stream_app->has_frame = false;
        s_stream_app->cur_fps = 0;
        s_stream_app->fps_window_frames = 0;
        s_stream_app->has_rssi = false;
        // Ensure advertising restarts immediately so PC can reconnect without re-entering app
        ble_adv_start();
    } else {
        s_stream_app->fps_window_ticks = app_timer_cnt_get();
        s_stream_app->fps_window_frames = 0;
        s_stream_app->has_rssi = ble_get_rssi(&s_stream_app->last_rssi);
    }
    mui_update(mui());
}

static void stream_on_ble_conn_evt(bool connected) {
    uint8_t flag = connected ? 1 : 0;
    app_sched_event_put(&flag, sizeof(flag), stream_conn_sched_handler);
}

static void stream_decode_rle(const uint8_t *src, uint16_t src_len, uint8_t *dst) {
    uint16_t si = 0;
    uint16_t di = 0;
    while (si < src_len && di < STREAM_FRAME_BYTES) {
        uint8_t ctrl = src[si++];
        if (ctrl < 128) {
            uint16_t count = (uint16_t)ctrl + 1;
            while (count-- > 0 && si < src_len && di < STREAM_FRAME_BYTES) {
                dst[di++] = src[si++];
            }
        } else {
            uint16_t count = (uint16_t)(ctrl - 128) + 2;
            if (si >= src_len) {
                break;
            }
            uint8_t val = src[si++];
            while (count-- > 0 && di < STREAM_FRAME_BYTES) {
                dst[di++] = val;
            }
        }
    }
    while (di < STREAM_FRAME_BYTES) {
        dst[di++] = 0;
    }
}

static void stream_decode_delta_rle(const uint8_t *src, uint16_t src_len, uint8_t *dst) {
    uint16_t si = 0;
    uint16_t di = 0;
    while (si < src_len && di < STREAM_FRAME_BYTES) {
        uint8_t ctrl = src[si++];
        if (ctrl < 128) {
            uint16_t count = (uint16_t)ctrl + 1;
            while (count-- > 0 && si < src_len && di < STREAM_FRAME_BYTES) {
                dst[di++] ^= src[si++];
            }
        } else {
            uint16_t count = (uint16_t)(ctrl - 128) + 2;
            if (si >= src_len) {
                break;
            }
            uint8_t val = src[si++];
            if (val == 0) {
                di += count;
            } else {
                while (count-- > 0 && di < STREAM_FRAME_BYTES) {
                    dst[di++] ^= val;
                }
            }
        }
    }
}

static void stream_on_ble_rx(const uint8_t *data, size_t data_len) {
    app_ble_stream_t *app = s_stream_app;
    if (app == NULL || data == NULL || data_len == 0) {
        return;
    }

    nrf_pwr_mgmt_feed();

    for (size_t i = 0; i < data_len; i++) {
        uint8_t b = data[i];
        switch (app->rx_state) {
        case RX_WAIT_MAGIC0:
            if (b == STREAM_MAGIC_0) {
                app->rx_state = RX_WAIT_MAGIC1;
            }
            break;

        case RX_WAIT_MAGIC1:
            if (b == STREAM_MAGIC_1) {
                app->rx_state = RX_WAIT_FLAGS;
            } else if (b != STREAM_MAGIC_0) {
                app->rx_state = RX_WAIT_MAGIC0;
            }
            break;

        case RX_WAIT_FLAGS:
            app->rx_flags = b;
            app->rx_state = RX_WAIT_LEN_LO;
            break;

        case RX_WAIT_LEN_LO:
            app->rx_expected_len = b;
            app->rx_state = RX_WAIT_LEN_HI;
            break;

        case RX_WAIT_LEN_HI:
            app->rx_expected_len |= ((uint16_t)b << 8);
            app->rx_pos = 0;
            if (app->rx_expected_len == 0 || app->rx_expected_len > STREAM_FRAME_BYTES) {
                app->rx_state = RX_WAIT_MAGIC0;
            } else {
                app->rx_state = RX_WAIT_PAYLOAD;
            }
            break;

        case RX_WAIT_PAYLOAD:
            app->rx_buf[app->rx_pos++] = b;
            if (app->rx_pos >= app->rx_expected_len) {
                if (app->rx_flags == STREAM_FLAG_DELTA_RLE && app->has_frame) {
                    stream_decode_delta_rle(app->rx_buf, app->rx_expected_len, app->frame_buf);
                } else {
                    if (app->rx_flags == STREAM_FLAG_RLE) {
                        stream_decode_rle(app->rx_buf, app->rx_expected_len, app->frame_buf);
                    } else if (app->rx_expected_len == STREAM_FRAME_BYTES) {
                        memcpy(app->frame_buf, app->rx_buf, STREAM_FRAME_BYTES);
                    }
                    if (app->invert) {
                        for (uint16_t k = 0; k < STREAM_FRAME_BYTES; k++) {
                            app->frame_buf[k] ^= 0xFF;
                        }
                    }
                }
                app->has_frame = true;
                app->connected = true;
                app->frame_count++;
                app->last_pkt_len = app->rx_expected_len;

                app->fps_window_frames++;
                uint32_t now = app_timer_cnt_get();
                uint32_t diff = app_timer_cnt_diff_compute(now, app->fps_window_ticks);
                if (diff >= APP_TIMER_TICKS(1000)) {
                    app->cur_fps = app->fps_window_frames;
                    app->fps_window_frames = 0;
                    app->fps_window_ticks = now;
                    app->has_rssi = ble_get_rssi(&app->last_rssi);
                    if (app->has_rssi) {
                        char rssi_msg[16];
                        int n = snprintf(rssi_msg, sizeof(rssi_msg), "RSSI:%d\n", (int)app->last_rssi);
                        if (n > 0) {
                            (void)ble_nus_tx_data(rssi_msg, (size_t)n);
                        }
                    }
                }

                app->rx_state = RX_WAIT_MAGIC0;
                stream_request_redraw(app);
            }
            break;
        }
    }
}

static void stream_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    app_ble_stream_t *app = p_view->user_data;
    if (app->has_frame) {
        mui_canvas_set_draw_color(p_canvas, 1);
        mui_canvas_draw_xbm(p_canvas, 0, 0, 128, 64, app->frame_buf);

        if (app->show_hud || !app->connected) {
            char hud[32];
            mui_canvas_set_draw_color(p_canvas, 0);
            mui_canvas_draw_box(p_canvas, 0, 0, 128, 12);
            mui_canvas_set_draw_color(p_canvas, 1);
            mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
            if (!app->connected) {
                snprintf(hud, sizeof(hud), "WAIT %s", app->ble_addr + 9);
            } else if (app->has_rssi) {
                snprintf(hud, sizeof(hud), "%uFPS %ddBm %uB%s",
                         (unsigned)app->cur_fps,
                         (int)app->last_rssi,
                         (unsigned)app->last_pkt_len,
                         app->invert ? " INV" : "");
            } else {
                snprintf(hud, sizeof(hud), "%uFPS %uB%s",
                         (unsigned)app->cur_fps,
                         (unsigned)app->last_pkt_len,
                         app->invert ? " INV" : "");
            }
            mui_canvas_draw_utf8(p_canvas, 2, 10, hud);
        }
        return;
    }

    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    mui_canvas_draw_utf8(p_canvas, 22, 14, stream_tr("BLE 蓝牙投屏", "BLE Stream"));
    mui_canvas_draw_utf8(p_canvas, 14, 30, app->ble_addr);
    mui_canvas_draw_utf8(p_canvas, 4, 46,
                         app->connected ? stream_tr("已连接 等待画面...", "Connected, Waiting...")
                                        : stream_tr("等待连接 (自动重连)", "Waiting (Auto-Reconn)"));
    mui_canvas_draw_utf8(p_canvas, 4, 60, stream_tr("左:调试 右:反色 中:退", "L:HUD R:Inv C:Exit"));
}

static void stream_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    app_ble_stream_t *app = p_view->user_data;
    if (event->type != INPUT_TYPE_SHORT && event->type != INPUT_TYPE_LONG) {
        return;
    }
    switch (event->key) {
    case INPUT_KEY_LEFT:
        app->show_hud = !app->show_hud;
        break;
    case INPUT_KEY_RIGHT:
        app->invert = !app->invert;
        if (app->has_frame) {
            for (uint16_t k = 0; k < STREAM_FRAME_BYTES; k++) {
                app->frame_buf[k] ^= 0xFF;
            }
        }
        break;
    case INPUT_KEY_CENTER:
        ble_set_conn_evt_handler(NULL);
        ble_nus_set_handler(NULL, NULL);
        ble_disable();
        mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_BLE_STREAM);
        break;
    default:
        break;
    }
}

static void stream_view_on_enter(mui_view_t *p_view) { (void)p_view; }
static void stream_view_on_exit(mui_view_t *p_view) { (void)p_view; }

static void app_ble_stream_on_run(mini_app_inst_t *p_app_inst) {
    app_ble_stream_t *app = mui_mem_malloc(sizeof(app_ble_stream_t));
    memset(app, 0, sizeof(*app));
    p_app_inst->p_handle = app;
    s_stream_app = app;

    ble_init();
    ble_device_mode_prepare(BLE_DEVICE_MODE_PIXLJS);
    ble_set_conn_evt_handler(stream_on_ble_conn_evt);
    ble_nus_set_handler(stream_on_ble_rx, NULL);
    ble_adv_start();
    ble_get_addr_str(app->ble_addr);

    app->p_view = mui_view_create();
    app->p_view->user_data = app;
    app->p_view->draw_cb = stream_view_on_draw;
    app->p_view->input_cb = stream_view_on_input;
    app->p_view->enter_cb = stream_view_on_enter;
    app->p_view->exit_cb = stream_view_on_exit;

    app->p_view_dispatcher = mui_view_dispatcher_create();
    mui_view_dispatcher_add_view(app->p_view_dispatcher, STREAM_VIEW_ID_MAIN, app->p_view);
    mui_view_dispatcher_attach(app->p_view_dispatcher, MUI_LAYER_FULLSCREEN);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, STREAM_VIEW_ID_MAIN);
}

static void app_ble_stream_on_kill(mini_app_inst_t *p_app_inst) {
    app_ble_stream_t *app = p_app_inst->p_handle;
    s_stream_app = NULL;
    ble_set_conn_evt_handler(NULL);
    ble_nus_set_handler(NULL, NULL);
    ble_disable();

    mui_view_dispatcher_detach(app->p_view_dispatcher, MUI_LAYER_FULLSCREEN);
    mui_view_dispatcher_free(app->p_view_dispatcher);
    mui_view_free(app->p_view);
    mui_mem_free(app);
    p_app_inst->p_handle = NULL;
}

static void app_ble_stream_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event) {
    (void)p_app_inst;
    (void)p_event;
}

// 32x32 TV / Play Screen icon (XBM)
static const uint8_t app_ble_stream_32x32_data[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xf0, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x1f, 0x1c, 0x00, 0x00, 0x38, 0x0c, 0x00, 0x00, 0x30,
    0x0c, 0x00, 0x00, 0x30, 0x0c, 0x0c, 0x00, 0x30, 0x0c, 0x3c, 0x00, 0x30, 0x0c, 0xfc, 0x00, 0x30,
    0x0c, 0xfc, 0x03, 0x30, 0x0c, 0xfc, 0x0f, 0x30, 0x0c, 0xfc, 0x1f, 0x30, 0x0c, 0xfc, 0x1f, 0x30,
    0x0c, 0xfc, 0x0f, 0x30, 0x0c, 0xfc, 0x03, 0x30, 0x0c, 0xfc, 0x00, 0x30, 0x0c, 0x3c, 0x00, 0x30,
    0x0c, 0x0c, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x30, 0x1c, 0x00, 0x00, 0x38, 0xf8, 0xff, 0xff, 0x1f,
    0xf0, 0xff, 0xff, 0x0f, 0x00, 0x80, 0x01, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0xfc, 0x3f, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const xbm_t app_ble_stream_32x32 = {
    .width = 32,
    .height = 32,
    .data = app_ble_stream_32x32_data,
};

const mini_app_t app_ble_stream_info = {
    .id = MINI_APP_ID_BLE_STREAM,
    .name = "蓝牙投屏",
    .name_i18n_key = _L_APP_BLE_STREAM,
    .icon = 0xe1b5,
    .deamon = false,
    .sys = false,
    .hibernate_enabled = false,
    .icon_32x32 = &app_ble_stream_32x32,
    .run_cb = app_ble_stream_on_run,
    .kill_cb = app_ble_stream_on_kill,
    .on_event_cb = app_ble_stream_on_event,
};
