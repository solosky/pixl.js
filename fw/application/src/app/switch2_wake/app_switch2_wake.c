#include "app_switch2_wake.h"

#include "ble_main.h"
#include "i18n/language.h"
#include "mini_app_launcher.h"
#include "mini_app_registry.h"
#include "mui_include.h"
#include "mui_list_view.h"
#include "mui_text_input.h"
#include "mui_toast_view.h"
#include "nrf_error.h"
#include "nrf_log.h"
#include "nrf_power.h"
#include "vfs.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SWITCH2_WAKE_CFG_PATH "/switch2_wake.txt"
#define SWITCH2_WAKE_ADV_DURATION_10MS 300 // 3.0 seconds
#define SWITCH2_WAKE_ADV_LEN 31

#define SWITCH2_PID_JC2_L 0x2066
#define SWITCH2_PID_JC2_R 0x2067
#define SWITCH2_PID_PRO2  0x2069

#define ICON_WAKE 0xe1b5
#define ICON_EDIT 0xe1ed
#define ICON_MODE 0xe02b
#define ICON_SAVE 0xe1d6
#define ICON_BACK 0xe069

typedef enum {
    SWITCH2_WAKE_VIEW_LIST = 0,
    SWITCH2_WAKE_VIEW_TEXT_INPUT = 1,
    SWITCH2_WAKE_VIEW_TOAST = 2,
} switch2_wake_view_id_t;

typedef enum {
    SWITCH2_MENU_WAKE = 0,
    SWITCH2_MENU_SCAN,
    SWITCH2_MENU_AUTO_WAKE,
    SWITCH2_MENU_EDIT_MAC,
    SWITCH2_MENU_EDIT_HOST,
    SWITCH2_MENU_PID,
    SWITCH2_MENU_MODE,
    SWITCH2_MENU_SAVE_CFG,
    SWITCH2_MENU_EXIT,
} switch2_menu_item_t;

typedef enum {
    SWITCH2_EDIT_NONE = 0,
    SWITCH2_EDIT_MAC,
    SWITCH2_EDIT_HOST,
} switch2_edit_target_t;

typedef struct {
    bool has_mac;
    uint8_t controller_mac[6];
    bool has_host;
    uint8_t host_mac[6];
    uint16_t pid;
    bool has_raw_data;
    uint8_t raw_data[SWITCH2_WAKE_ADV_LEN];
    uint8_t raw_data_len;
    bool connectable;
    bool auto_wake;
} switch2_wake_cfg_t;

typedef struct {
    mui_view_dispatcher_t *p_view_dispatcher;
    mui_view_dispatcher_t *p_view_dispatcher_toast;
    mui_list_view_t *p_list_view;
    mui_text_input_t *p_text_input;
    mui_toast_view_t *p_toast_view;
    switch2_wake_cfg_t cfg;
    switch2_edit_target_t edit_target;
    bool waking;
    bool scanning;
} app_switch2_wake_t;

static app_switch2_wake_t *s_active_app = NULL;

static const char *s2_tr(const char *zh, const char *en) {
    Language lang = getLanguage();
    return (lang == LANGUAGE_ZH_HANS || lang == LANGUAGE_ZH_TW) ? zh : en;
}

static int hex_nibble(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static bool parse_mac_str(const char *str, uint8_t out[6]) {
    uint8_t nibbles[12];
    int count = 0;
    for (const char *p = str; *p != '\0'; p++) {
        if (*p == ':' || *p == '-' || *p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') {
            continue;
        }
        int v = hex_nibble(*p);
        if (v < 0 || count >= 12) {
            return false;
        }
        nibbles[count++] = (uint8_t)v;
    }
    if (count != 12) {
        return false;
    }
    for (int i = 0; i < 6; i++) {
        out[i] = (uint8_t)((nibbles[i * 2] << 4) | nibbles[i * 2 + 1]);
    }
    return true;
}

static int parse_hex_bytes(const char *str, uint8_t *out, int max_len) {
    int hi = -1;
    int len = 0;
    for (const char *p = str; *p != '\0'; p++) {
        if (*p == ' ' || *p == ':' || *p == '-' || *p == '\t' || *p == '\r' || *p == '\n') {
            continue;
        }
        int v = hex_nibble(*p);
        if (v < 0) {
            return -1;
        }
        if (hi < 0) {
            hi = v;
        } else {
            if (len >= max_len) {
                return -1;
            }
            out[len++] = (uint8_t)((hi << 4) | v);
            hi = -1;
        }
    }
    if (hi >= 0) {
        return -1;
    }
    return len;
}

static void format_mac_colon(const uint8_t mac[6], char out[18]) {
    snprintf(out, 18, "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static void format_mac_plain(const uint8_t mac[6], char out[13]) {
    snprintf(out, 13, "%02X%02X%02X%02X%02X%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static void switch2_wake_cfg_set_default(switch2_wake_cfg_t *cfg) {
    memset(cfg, 0, sizeof(*cfg));
    cfg->pid = SWITCH2_PID_JC2_L;
    cfg->connectable = false;
    cfg->auto_wake = false;
}

static vfs_driver_t *switch2_wake_get_vfs(void) {
    vfs_driver_t *p_driver = vfs_get_default_driver();
    if (p_driver == NULL) {
        return NULL;
    }
    if (!p_driver->mounted()) {
        p_driver->mount();
    }
    if (!p_driver->mounted()) {
        return NULL;
    }
    return p_driver;
}

static void switch2_wake_load_cfg(switch2_wake_cfg_t *cfg) {
    switch2_wake_cfg_set_default(cfg);

    vfs_driver_t *p_driver = switch2_wake_get_vfs();
    if (p_driver == NULL) {
        return;
    }

    char buf[512];
    memset(buf, 0, sizeof(buf));
    int32_t read_len = p_driver->read_file_data(SWITCH2_WAKE_CFG_PATH, buf, sizeof(buf) - 1);
    if (read_len <= 0) {
        return;
    }
    buf[read_len] = '\0';

    char *line = buf;
    while (line != NULL && *line != '\0') {
        char *next = strchr(line, '\n');
        if (next != NULL) {
            *next = '\0';
            next++;
        }

        while (*line == ' ' || *line == '\t' || *line == '\r') {
            line++;
        }
        size_t l = strlen(line);
        while (l > 0 && (line[l - 1] == '\r' || line[l - 1] == ' ' || line[l - 1] == '\t')) {
            line[--l] = '\0';
        }

        if (*line != '\0' && *line != '#' && *line != ';') {
            char *eq = strchr(line, '=');
            if (eq != NULL) {
                *eq = '\0';
                char *key = line;
                char *val = eq + 1;
                while (*val == ' ' || *val == '\t') {
                    val++;
                }

                if (strcmp(key, "mac") == 0) {
                    if (parse_mac_str(val, cfg->controller_mac)) {
                        cfg->has_mac = true;
                    }
                } else if (strcmp(key, "host") == 0) {
                    if (parse_mac_str(val, cfg->host_mac)) {
                        cfg->has_host = true;
                    }
                } else if (strcmp(key, "pid") == 0) {
                    uint8_t pid_bytes[2];
                    if (parse_hex_bytes(val, pid_bytes, 2) == 2) {
                        cfg->pid = (uint16_t)((pid_bytes[0] << 8) | pid_bytes[1]);
                    }
                } else if (strcmp(key, "data") == 0) {
                    int n = parse_hex_bytes(val, cfg->raw_data, SWITCH2_WAKE_ADV_LEN);
                    if (n > 0) {
                        cfg->raw_data_len = (uint8_t)n;
                        cfg->has_raw_data = true;
                    }
                } else if (strcmp(key, "mode") == 0) {
                    if (strcmp(val, "conn") == 0) {
                        cfg->connectable = true;
                    } else if (strcmp(val, "nonconn") == 0) {
                        cfg->connectable = false;
                    }
                } else if (strcmp(key, "auto_wake") == 0) {
                    cfg->auto_wake = (val[0] == '1');
                }
            }
        }
        line = next;
    }
}

static bool switch2_wake_save_cfg(const switch2_wake_cfg_t *cfg) {
    vfs_driver_t *p_driver = switch2_wake_get_vfs();
    if (p_driver == NULL) {
        return false;
    }

    char buf[384];
    int pos = 0;
    char mac_str[18];

    pos += snprintf(buf + pos, sizeof(buf) - pos, "# Switch 2 BLE Wake Config\n");

    if (cfg->has_mac) {
        format_mac_colon(cfg->controller_mac, mac_str);
        pos += snprintf(buf + pos, sizeof(buf) - pos, "mac=%s\n", mac_str);
    } else {
        pos += snprintf(buf + pos, sizeof(buf) - pos, "#mac=78:81:8C:05:0F:FA\n");
    }

    if (cfg->has_host) {
        format_mac_colon(cfg->host_mac, mac_str);
        pos += snprintf(buf + pos, sizeof(buf) - pos, "host=%s\n", mac_str);
    } else {
        pos += snprintf(buf + pos, sizeof(buf) - pos, "#host=98:E2:55:99:1F:97\n");
    }

    pos += snprintf(buf + pos, sizeof(buf) - pos, "pid=%04X\n", cfg->pid);
    pos += snprintf(buf + pos, sizeof(buf) - pos, "mode=%s\n", cfg->connectable ? "conn" : "nonconn");
    pos += snprintf(buf + pos, sizeof(buf) - pos, "auto_wake=%d\n", cfg->auto_wake ? 1 : 0);

    if (cfg->has_raw_data && cfg->raw_data_len > 0) {
        pos += snprintf(buf + pos, sizeof(buf) - pos, "data=");
        for (int i = 0; i < cfg->raw_data_len && pos < (int)sizeof(buf) - 4; i++) {
            pos += snprintf(buf + pos, sizeof(buf) - pos, "%02X", cfg->raw_data[i]);
        }
        pos += snprintf(buf + pos, sizeof(buf) - pos, "\n");
    } else {
        pos += snprintf(buf + pos, sizeof(buf) - pos,
                        "#data=0201061BFF53050100037E0566200001810917158C81780F00000000000000\n");
    }

    int32_t err = p_driver->write_file_data(SWITCH2_WAKE_CFG_PATH, buf, (size_t)pos);
    return err >= 0;
}

static uint8_t switch2_wake_build_adv(const switch2_wake_cfg_t *cfg, uint8_t out[SWITCH2_WAKE_ADV_LEN]) {
    if (cfg->has_raw_data && cfg->raw_data_len > 0) {
        memcpy(out, cfg->raw_data, cfg->raw_data_len);
        return cfg->raw_data_len;
    }

    if (!cfg->has_host) {
        return 0;
    }

    memset(out, 0, SWITCH2_WAKE_ADV_LEN);
    // AD Flags: LE General Discoverable, BR/EDR Not Supported
    out[0] = 0x02;
    out[1] = 0x01;
    out[2] = 0x06;
    // Manufacturer Specific Data: len=27 (0x1B), type=0xFF
    out[3] = 0x1B;
    out[4] = 0xFF;
    // Nintendo Company ID 0x0553 (little-endian)
    out[5] = 0x53;
    out[6] = 0x05;
    // Protocol header
    out[7] = 0x01;
    out[8] = 0x00;
    out[9] = 0x03;
    // Nintendo VID 0x057E (little-endian)
    out[10] = 0x7E;
    out[11] = 0x05;
    // Controller PID (little-endian)
    out[12] = (uint8_t)(cfg->pid & 0xFF);
    out[13] = (uint8_t)((cfg->pid >> 8) & 0xFF);
    // State flags: 00 01 81 (0x81 = Wake from sleep)
    out[14] = 0x00;
    out[15] = 0x01;
    out[16] = 0x81;
    // Target Switch 2 MAC address (reversed byte order)
    out[17] = cfg->host_mac[5];
    out[18] = cfg->host_mac[4];
    out[19] = cfg->host_mac[3];
    out[20] = cfg->host_mac[2];
    out[21] = cfg->host_mac[1];
    out[22] = cfg->host_mac[0];
    // Constant + padding zeros
    out[23] = 0x0F;

    return SWITCH2_WAKE_ADV_LEN;
}

static const char *pid_label(uint16_t pid) {
    if (pid == SWITCH2_PID_JC2_L) return "JC2-L";
    if (pid == SWITCH2_PID_JC2_R) return "JC2-R";
    if (pid == SWITCH2_PID_PRO2)  return "Pro2";
    return "Custom";
}

static void switch2_wake_reload_menu(app_switch2_wake_t *app) {
    uint16_t focus = mui_list_view_get_focus(app->p_list_view);
    uint16_t scroll = mui_list_view_get_scroll_offset(app->p_list_view);
    mui_list_view_clear_items(app->p_list_view);

    char sub[16];

    mui_list_view_add_item_ext(app->p_list_view, ICON_WAKE,
                               app->waking ? s2_tr("正在唤醒...", "Waking...") : s2_tr("唤醒 Switch 2", "Wake Switch 2"),
                               app->waking ? "..." : "",
                               (void *)SWITCH2_MENU_WAKE);

    mui_list_view_add_item_ext(app->p_list_view, ICON_WAKE,
                               app->scanning ? s2_tr("正在扫描...", "Scanning...") : s2_tr("扫描唤醒包", "Scan Wake Pkt"),
                               app->scanning ? "15s" : "",
                               (void *)SWITCH2_MENU_SCAN);

    mui_list_view_add_item_ext(app->p_list_view, ICON_MODE,
                               s2_tr("开屏自动唤醒", "Boot Wake"),
                               app->cfg.auto_wake ? _T(ON_F) : _T(OFF_F),
                               (void *)SWITCH2_MENU_AUTO_WAKE);

    if (app->cfg.has_mac) {
        snprintf(sub, sizeof(sub), "..%02X%02X%02X",
                 app->cfg.controller_mac[3], app->cfg.controller_mac[4], app->cfg.controller_mac[5]);
    } else {
        snprintf(sub, sizeof(sub), "%s", s2_tr("未设置", "Unset"));
    }
    mui_list_view_add_item_ext(app->p_list_view, ICON_EDIT, s2_tr("手柄 MAC", "Ctrl MAC"), sub,
                               (void *)SWITCH2_MENU_EDIT_MAC);

    if (app->cfg.has_raw_data) {
        snprintf(sub, sizeof(sub), "RAW %dB", app->cfg.raw_data_len);
    } else if (app->cfg.has_host) {
        snprintf(sub, sizeof(sub), "..%02X%02X%02X",
                 app->cfg.host_mac[3], app->cfg.host_mac[4], app->cfg.host_mac[5]);
    } else {
        snprintf(sub, sizeof(sub), "%s", s2_tr("未设置", "Unset"));
    }
    mui_list_view_add_item_ext(app->p_list_view, ICON_EDIT, s2_tr("主机 MAC", "Host MAC"), sub,
                               (void *)SWITCH2_MENU_EDIT_HOST);

    mui_list_view_add_item_ext(app->p_list_view, ICON_MODE, s2_tr("手柄类型", "Ctrl Type"), pid_label(app->cfg.pid),
                               (void *)SWITCH2_MENU_PID);

    mui_list_view_add_item_ext(app->p_list_view, ICON_MODE, s2_tr("广播模式", "Adv Mode"),
                               app->cfg.connectable ? "Conn" : "NonConn",
                               (void *)SWITCH2_MENU_MODE);

    mui_list_view_add_item(app->p_list_view, ICON_SAVE, s2_tr("保存配置", "Save Config"),
                           (void *)SWITCH2_MENU_SAVE_CFG);

    mui_list_view_add_item(app->p_list_view, ICON_BACK, _T(MAIN_RETURN),
                           (void *)SWITCH2_MENU_EXIT);

    uint32_t count = mui_list_view_item_size(app->p_list_view);
    if (focus >= count && count > 0) {
        focus = (uint16_t)(count - 1);
    }
    mui_list_view_set_focus(app->p_list_view, focus);
    mui_list_view_set_scroll_offset(app->p_list_view, scroll);
}

static void switch2_wake_on_ble_done(void) {
    if (s_active_app == NULL) {
        return;
    }
    s_active_app->waking = false;
    switch2_wake_reload_menu(s_active_app);
    mui_toast_view_show(s_active_app->p_toast_view, s2_tr("唤醒包已发送", "Wake Sent"));
    mui_update(mui());
}

static void switch2_wake_on_scan_done(bool found, const ble_switch2_scan_result_t *res) {
    if (s_active_app == NULL) {
        return;
    }
    s_active_app->scanning = false;
    if (found && res != NULL) {
        memcpy(s_active_app->cfg.controller_mac, res->controller_mac, 6);
        s_active_app->cfg.has_mac = true;
        memcpy(s_active_app->cfg.host_mac, res->host_mac, 6);
        s_active_app->cfg.has_host = true;
        s_active_app->cfg.pid = res->pid;
        memcpy(s_active_app->cfg.raw_data, res->adv_data, res->adv_len);
        s_active_app->cfg.raw_data_len = res->adv_len;
        s_active_app->cfg.has_raw_data = true;
        bool saved = switch2_wake_save_cfg(&s_active_app->cfg);
        switch2_wake_reload_menu(s_active_app);
        mui_toast_view_show(s_active_app->p_toast_view,
                            saved ? s2_tr("捕获成功!", "Captured!") : _T(FAILED));
    } else {
        switch2_wake_reload_menu(s_active_app);
        mui_toast_view_show(s_active_app->p_toast_view, s2_tr("扫描超时", "Scan Timeout"));
    }
    mui_update(mui());
}

static void switch2_wake_toggle_scan(app_switch2_wake_t *app) {
    if (app->scanning) {
        ble_switch2_scan_stop();
        app->scanning = false;
        switch2_wake_reload_menu(app);
        mui_toast_view_show(app->p_toast_view, s2_tr("已停止扫描", "Scan Stopped"));
        return;
    }

    if (app->waking) {
        ble_raw_adv_stop();
        app->waking = false;
    }

    uint32_t err = ble_switch2_scan_start(15, switch2_wake_on_scan_done);
    if (err != NRF_SUCCESS) {
        char msg[32];
        snprintf(msg, sizeof(msg), "Scan Err: 0x%X", (unsigned int)err);
        mui_toast_view_show(app->p_toast_view, msg);
        return;
    }

    app->scanning = true;
    switch2_wake_reload_menu(app);
    mui_toast_view_show(app->p_toast_view, s2_tr("请按手柄Home键", "Press JC2 Home"));
}

static void switch2_wake_trigger(app_switch2_wake_t *app) {
    if (app->scanning) {
        ble_switch2_scan_stop();
        app->scanning = false;
    }
    if (app->waking) {
        ble_raw_adv_stop();
        app->waking = false;
        switch2_wake_reload_menu(app);
        mui_toast_view_show(app->p_toast_view, s2_tr("已停止唤醒", "Stopped"));
        return;
    }

    if (!app->cfg.has_mac) {
        mui_toast_view_show(app->p_toast_view, s2_tr("请先设置手柄MAC", "Set Ctrl MAC"));
        return;
    }

    uint8_t adv_payload[SWITCH2_WAKE_ADV_LEN];
    uint8_t adv_len = switch2_wake_build_adv(&app->cfg, adv_payload);
    if (adv_len == 0) {
        mui_toast_view_show(app->p_toast_view, s2_tr("请先设置主机MAC", "Set Host MAC"));
        return;
    }

    uint32_t err = ble_raw_adv_start(app->cfg.controller_mac, adv_payload, adv_len,
                                     app->cfg.connectable, SWITCH2_WAKE_ADV_DURATION_10MS,
                                     switch2_wake_on_ble_done);
    if (err != NRF_SUCCESS) {
        char msg[32];
        snprintf(msg, sizeof(msg), "BLE Err: 0x%X", (unsigned int)err);
        mui_toast_view_show(app->p_toast_view, msg);
        return;
    }

    app->waking = true;
    switch2_wake_reload_menu(app);
    mui_toast_view_show(app->p_toast_view, s2_tr("正在唤醒...", "Waking..."));
}

static void switch2_wake_text_input_cb(mui_text_input_event_t event, mui_text_input_t *p_text_input) {
    app_switch2_wake_t *app = p_text_input->user_data;

    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        const char *txt = mui_text_input_get_input_text(p_text_input);
        uint8_t mac[6];
        if (!parse_mac_str(txt, mac)) {
            mui_toast_view_show(app->p_toast_view, s2_tr("MAC格式错误!", "Invalid MAC!"));
            mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SWITCH2_WAKE_VIEW_LIST);
            app->edit_target = SWITCH2_EDIT_NONE;
            return;
        }

        if (app->edit_target == SWITCH2_EDIT_MAC) {
            memcpy(app->cfg.controller_mac, mac, 6);
            app->cfg.has_mac = true;
        } else if (app->edit_target == SWITCH2_EDIT_HOST) {
            memcpy(app->cfg.host_mac, mac, 6);
            app->cfg.has_host = true;
            app->cfg.has_raw_data = false;
            app->cfg.raw_data_len = 0;
        }

        bool saved = switch2_wake_save_cfg(&app->cfg);
        switch2_wake_reload_menu(app);
        mui_toast_view_show(app->p_toast_view,
                            saved ? s2_tr("配置已保存", "Config Saved") : _T(FAILED));
    }

    app->edit_target = SWITCH2_EDIT_NONE;
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SWITCH2_WAKE_VIEW_LIST);
}

static void switch2_wake_open_mac_editor(app_switch2_wake_t *app, switch2_edit_target_t target) {
    char initial[13] = "";
    app->edit_target = target;

    mui_text_input_reset(app->p_text_input);
    if (target == SWITCH2_EDIT_MAC) {
        mui_text_input_set_header(app->p_text_input, s2_tr("手柄 MAC", "Ctrl MAC"));
        if (app->cfg.has_mac) {
            format_mac_plain(app->cfg.controller_mac, initial);
        }
    } else {
        mui_text_input_set_header(app->p_text_input, s2_tr("主机 MAC", "Host MAC"));
        if (app->cfg.has_host) {
            format_mac_plain(app->cfg.host_mac, initial);
        }
    }
    mui_text_input_set_input_text(app->p_text_input, initial);
    mui_text_input_set_event_cb(app->p_text_input, switch2_wake_text_input_cb);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SWITCH2_WAKE_VIEW_TEXT_INPUT);
}

static void switch2_wake_list_selected_cb(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                          mui_list_item_t *p_item) {
    app_switch2_wake_t *app = p_list_view->user_data;
    uint32_t sel = (uint32_t)p_item->user_data;

    switch (sel) {
    case SWITCH2_MENU_WAKE:
        switch2_wake_trigger(app);
        break;
    case SWITCH2_MENU_SCAN:
        switch2_wake_toggle_scan(app);
        break;
    case SWITCH2_MENU_AUTO_WAKE:
        app->cfg.auto_wake = !app->cfg.auto_wake;
        switch2_wake_save_cfg(&app->cfg);
        switch2_wake_reload_menu(app);
        break;
    case SWITCH2_MENU_EDIT_MAC:
        switch2_wake_open_mac_editor(app, SWITCH2_EDIT_MAC);
        break;
    case SWITCH2_MENU_EDIT_HOST:
        switch2_wake_open_mac_editor(app, SWITCH2_EDIT_HOST);
        break;
    case SWITCH2_MENU_PID:
        if (app->cfg.pid == SWITCH2_PID_JC2_L) {
            app->cfg.pid = SWITCH2_PID_JC2_R;
        } else if (app->cfg.pid == SWITCH2_PID_JC2_R) {
            app->cfg.pid = SWITCH2_PID_PRO2;
        } else {
            app->cfg.pid = SWITCH2_PID_JC2_L;
        }
        app->cfg.has_raw_data = false;
        app->cfg.raw_data_len = 0;
        switch2_wake_save_cfg(&app->cfg);
        switch2_wake_reload_menu(app);
        break;
    case SWITCH2_MENU_MODE:
        app->cfg.connectable = !app->cfg.connectable;
        switch2_wake_save_cfg(&app->cfg);
        switch2_wake_reload_menu(app);
        break;
    case SWITCH2_MENU_SAVE_CFG:
        if (switch2_wake_save_cfg(&app->cfg)) {
            mui_toast_view_show(app->p_toast_view, s2_tr("配置已保存", "Config Saved"));
        } else {
            mui_toast_view_show(app->p_toast_view, _T(FAILED));
        }
        break;
    case SWITCH2_MENU_EXIT:
        ble_switch2_scan_stop();
        ble_raw_adv_stop();
        mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_SWITCH2_WAKE);
        break;
    default:
        break;
    }
}

static void app_switch2_wake_on_run(mini_app_inst_t *p_app_inst) {
    app_switch2_wake_t *app = mui_mem_malloc(sizeof(app_switch2_wake_t));
    memset(app, 0, sizeof(*app));
    p_app_inst->p_handle = app;
    s_active_app = app;

    switch2_wake_load_cfg(&app->cfg);

    app->p_view_dispatcher = mui_view_dispatcher_create();
    app->p_list_view = mui_list_view_create();
    mui_list_view_set_user_data(app->p_list_view, app);
    mui_list_view_set_selected_cb(app->p_list_view, switch2_wake_list_selected_cb);

    app->p_text_input = mui_text_input_create();
    mui_text_input_set_user_data(app->p_text_input, app);

    mui_view_dispatcher_add_view(app->p_view_dispatcher, SWITCH2_WAKE_VIEW_LIST,
                                 mui_list_view_get_view(app->p_list_view));
    mui_view_dispatcher_add_view(app->p_view_dispatcher, SWITCH2_WAKE_VIEW_TEXT_INPUT,
                                 mui_text_input_get_view(app->p_text_input));
    mui_view_dispatcher_attach(app->p_view_dispatcher, MUI_LAYER_WINDOW);

    app->p_toast_view = mui_toast_view_create();
    mui_toast_view_set_user_data(app->p_toast_view, app);
    app->p_view_dispatcher_toast = mui_view_dispatcher_create();
    mui_view_dispatcher_add_view(app->p_view_dispatcher_toast, SWITCH2_WAKE_VIEW_TOAST,
                                 mui_toast_view_get_view(app->p_toast_view));
    mui_view_dispatcher_attach(app->p_view_dispatcher_toast, MUI_LAYER_TOAST);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher_toast, SWITCH2_WAKE_VIEW_TOAST);

    switch2_wake_reload_menu(app);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SWITCH2_WAKE_VIEW_LIST);
}

static void app_switch2_wake_on_kill(mini_app_inst_t *p_app_inst) {
    app_switch2_wake_t *app = p_app_inst->p_handle;
    s_active_app = NULL;
    ble_switch2_scan_stop();
    ble_raw_adv_stop();

    mui_view_dispatcher_detach(app->p_view_dispatcher, MUI_LAYER_WINDOW);
    mui_view_dispatcher_free(app->p_view_dispatcher);
    mui_list_view_free(app->p_list_view);
    mui_text_input_free(app->p_text_input);

    mui_view_dispatcher_detach(app->p_view_dispatcher_toast, MUI_LAYER_TOAST);
    mui_view_dispatcher_free(app->p_view_dispatcher_toast);
    mui_toast_view_free(app->p_toast_view);

    mui_mem_free(app);
    p_app_inst->p_handle = NULL;
}

static void app_switch2_wake_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event) {}

void app_switch2_wake_on_boot(uint32_t wakeup_reason) {
    // Skip silent RFID-field wakeups (e.g., scanning an Amiibo card without turning on screen)
    if (wakeup_reason & (NRF_POWER_RESETREAS_NFC_MASK | NRF_POWER_RESETREAS_LPCOMP_MASK)) {
        return;
    }

    // Do not clobber BLE state if a non-desktop app (e.g. AmiiboLink) was restored on boot
    mini_app_launcher_t *p_launcher = mini_app_launcher();
    if (p_launcher->p_main_app_inst != NULL && p_launcher->p_main_app_inst->p_app != NULL &&
        p_launcher->p_main_app_inst->p_app->id != MINI_APP_ID_DESKTOP) {
        return;
    }

    switch2_wake_cfg_t cfg;
    switch2_wake_load_cfg(&cfg);
    if (!cfg.auto_wake || !cfg.has_mac) {
        return;
    }

    uint8_t adv_payload[SWITCH2_WAKE_ADV_LEN];
    uint8_t adv_len = switch2_wake_build_adv(&cfg, adv_payload);
    if (adv_len == 0) {
        return;
    }

    (void)ble_raw_adv_start(cfg.controller_mac, adv_payload, adv_len,
                            cfg.connectable, 200, NULL);
}

static const uint8_t app_switch2_wake_32x32_data[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x3f, 0x00, 0x00, 0x03, 0xc0, 0x00,
    0x00, 0xf0, 0x0f, 0x00, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0xfc, 0x01,
    0xe0, 0x3f, 0xfc, 0x07, 0x70, 0x20, 0xfc, 0x0f, 0x30, 0x20, 0xfc, 0x0f, 0x30, 0x27, 0xfc, 0x0f,
    0xb0, 0x2f, 0xfc, 0x0f, 0xb0, 0x2f, 0xfc, 0x0f, 0x30, 0x27, 0xfc, 0x0f, 0x30, 0x20, 0xfc, 0x0f,
    0x30, 0x20, 0xfc, 0x0f, 0x30, 0x20, 0xfc, 0x0f, 0x30, 0x20, 0x3c, 0x0f, 0x30, 0x20, 0x1c, 0x0e,
    0x30, 0x20, 0x1c, 0x0e, 0x30, 0x20, 0x3c, 0x0f, 0x30, 0x20, 0xfc, 0x0f, 0x30, 0x20, 0xfc, 0x0f,
    0x30, 0x20, 0xfc, 0x0f, 0x70, 0x20, 0xfc, 0x0f, 0xe0, 0x3f, 0xfc, 0x07, 0x80, 0x3f, 0xfc, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const xbm_t app_switch2_wake_32x32 = {
    .width = 32,
    .height = 32,
    .data = app_switch2_wake_32x32_data,
};

const mini_app_t app_switch2_wake_info = {
    .id = MINI_APP_ID_SWITCH2_WAKE,
    .name = "Switch2唤醒",
    .name_i18n_key = _L_APP_SWITCH2_WAKE,
    .icon = 0xe1b5,
    .deamon = false,
    .sys = false,
    .hibernate_enabled = false,
    .icon_32x32 = &app_switch2_wake_32x32,
    .run_cb = app_switch2_wake_on_run,
    .kill_cb = app_switch2_wake_on_kill,
    .on_event_cb = app_switch2_wake_on_event,
};
