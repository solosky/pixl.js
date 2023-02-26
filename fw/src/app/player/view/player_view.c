#include "player_view.h"
#include "nrf_log.h"
#include "nrf_pwr_mgmt.h"

static void player_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    player_view_t *p_player_view = p_view->user_data;
    if (p_player_view->is_playing) {
        size_t read = p_player_view->p_dirver->read_file(&p_player_view->fd, p_player_view->frame_buffer,
                                                         p_player_view->frame_buffer_size);
        if (read == p_player_view->frame_buffer_size) {

            uint8_t x = (mui_canvas_get_width(p_canvas) - p_player_view->header.width) / 2;
            uint8_t y = (mui_canvas_get_height(p_canvas) - p_player_view->header.height) / 2;
            mui_canvas_draw_xbm(p_canvas, x, y, p_player_view->header.width, p_player_view->header.height,
                                p_player_view->frame_buffer);
            p_player_view->played_frames++;

            char txt[10];
            sprintf(txt, "%d", p_player_view->played_frames++);

            mui_canvas_set_draw_color(p_canvas, 0);
            mui_canvas_draw_box(p_canvas, 0, 0, mui_canvas_get_utf8_width(p_canvas, txt), 12);
            mui_canvas_set_draw_color(p_canvas, 1);
            mui_canvas_draw_utf8(p_canvas, 0, 10, txt);
        } else {
            player_view_stop(p_player_view);
            if (p_player_view->event_cb) {
                p_player_view->event_cb(PLAYER_VIEW_EVENT_PLAY_STOPPED, p_player_view);
            }
        }
    } else {
        mui_canvas_draw_utf8(p_canvas, 0, 10, "<未播放动画>");
    }
}

static void player_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    player_view_t *p_player_view = p_view->user_data;
    if (p_player_view->event_cb) {
        p_player_view->event_cb(PLAYER_VIEW_EVENT_KEY_PRESSED, p_player_view);
    }
}

static void player_view_on_enter(mui_view_t *p_view) {}

static void player_view_on_exit(mui_view_t *p_view) {}

static void player_view_on_frame_tick(void *p_context) {
    player_view_t *p_view = p_context;
    nrf_pwr_mgmt_feed();
    mui_update(mui());
}

player_view_t *player_view_create() {
    player_view_t *p_player_view = mui_mem_malloc(sizeof(player_view_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_player_view;
    p_view->draw_cb = player_view_on_draw;
    p_view->input_cb = player_view_on_input;
    p_view->enter_cb = player_view_on_enter;
    p_view->exit_cb = player_view_on_exit;

    p_player_view->p_view = p_view;
    p_player_view->frame_buffer = NULL;

    p_player_view->frame_tick_timer_id = &p_player_view->frame_tick_timer_data;
    int32_t err_code =
        app_timer_create(&p_player_view->frame_tick_timer_id, APP_TIMER_MODE_REPEATED, player_view_on_frame_tick);
    APP_ERROR_CHECK(err_code);

    return p_player_view;
}
void player_view_free(player_view_t *p_view) {
    player_view_stop(p_view);
    mui_view_free(p_view->p_view);
    mui_mem_free(p_view);
}
mui_view_t *player_view_get_view(player_view_t *p_view) { return p_view->p_view; }

void player_view_play(player_view_t *p_view, vfs_drive_t drive, char *path) {

    vfs_driver_t *p_driver = vfs_get_driver(drive);
    int32_t err = p_driver->open_file(path, &p_view->fd, VFS_MODE_READONLY);
    if (err != VFS_OK) {
        NRF_LOG_INFO("open file failed");
        return;
    }

    err = p_driver->read_file(&p_view->fd, &p_view->header, sizeof(v_header_t));
    if (err < 0) {
        NRF_LOG_INFO("read file failed");
        p_driver->close_file(&p_view->fd);
        return;
    }
    NRF_LOG_INFO("magic:%X, frame_count:%d, fps:%d, width:%d, height:%d, reserved:%d", p_view->header.magic,
                 p_view->header.frame_count, p_view->header.fps, p_view->header.width, p_view->header.height,
                 p_view->header.reserved);

    if (p_view->header.magic != V_HEADER_MAGIC) {
        p_driver->close_file(&p_view->fd);
        return;
    }

    p_view->p_dirver = p_driver;
    p_view->played_frames = 0;

    if (p_view->header.width % 8) {
        p_view->frame_buffer_size = (p_view->header.width + 8) * p_view->header.height / 8;
    } else {
        p_view->frame_buffer_size = p_view->header.width * p_view->header.height / 8;
    }

    p_view->frame_buffer = mui_mem_malloc(p_view->frame_buffer_size);
    if (p_view->frame_buffer == NULL) {
        NRF_LOG_INFO("malloc frame buffer failed");
        p_driver->close_file(&p_view->fd);
        return;
    }

    p_view->is_playing = true;
    NRF_LOG_INFO("frame interval: %d ms", 1000 / p_view->header.fps);

    int32_t err_code =
        app_timer_start(p_view->frame_tick_timer_id, APP_TIMER_TICKS(1000 / p_view->header.fps), (void *)p_view);
    APP_ERROR_CHECK(err_code);
}

void player_view_stop(player_view_t *p_view) {
    if (p_view->is_playing) {
        p_view->is_playing = false;
        app_timer_stop(p_view->frame_tick_timer_id);
        if (p_view->frame_buffer) {
            mui_mem_free(p_view->frame_buffer);
            p_view->frame_buffer = NULL;
        }
        p_view->p_dirver->close_file(&p_view->fd);
    }
}