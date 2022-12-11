#include "amiibo_view.h"
#include "amiibo_data.h"
#include "mini_app_launcher.h"
#include "mini_app_registry.h"
#include "ntag_emu.h"
#include "ntag_store.h"

#include "app_amiibo.h"
#include "mui_core.h"
#include "nrf_log.h"
#include "vos.h"

static void amiibo_update_cb(void *context, ntag_t *p_ntag) {
    amiibo_view_t *p_amiibo_view = context;
    app_amiibo_t *p_app = p_amiibo_view->user_data;
    memcpy(p_amiibo_view->amiibo, p_ntag, sizeof(ntag_t));

    vos_driver_t *p_driver = vos_get_driver(p_app->current_drive);
    // save to fs
    int32_t res = p_driver->write_object(VOS_BUCKET_AMIIBO, string_get_cstr(p_app->current_folder),
                                         string_get_cstr(p_app->current_file), p_ntag, sizeof(ntag_t));
    if (res > 0) {
        uint32_t head = to_little_endian_int32(&p_ntag->data[84]);
        uint32_t tail = to_little_endian_int32(&p_ntag->data[88]);

        const amiibo_data_t *amd = find_amiibo_data(head, tail);

        if (amd) {
            char new_name[VOS_MAX_OBJECT_SIZE];
            snprintf(new_name, sizeof(new_name), "%s.%s", amd->name, "bin");
            res = p_driver->rename_object(VOS_BUCKET_AMIIBO, string_get_cstr(p_app->current_folder),
                                          string_get_cstr(p_app->current_file), new_name);
        }

        // todo ..
    }

    mui_update(mui());
}

static void amiibo_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    char buff[64];
    amiibo_view_t *p_amiibo_view = p_view->user_data;
    ntag_t *ntag = p_amiibo_view->amiibo;

    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    sprintf(buff, "%02d %02x:%02x:%02x:%02x:%02x:%02x:%02x", ntag->index + 1, ntag->data[0], ntag->data[1],
            ntag->data[2], ntag->data[4], ntag->data[5], ntag->data[6], ntag->data[7]);

    uint8_t y = 0;
    mui_canvas_draw_box(p_canvas, 0, y, mui_canvas_get_width(p_canvas), 12);
    mui_canvas_set_draw_color(p_canvas, 0);
    mui_canvas_set_font(p_canvas, u8g2_font_wqy12_t_gb2312a);
    mui_canvas_draw_utf8(p_canvas, 1, y += 10, buff);
    mui_canvas_set_draw_color(p_canvas, 1);

    uint32_t head = to_little_endian_int32(&ntag->data[84]);
    uint32_t tail = to_little_endian_int32(&ntag->data[88]);

    const amiibo_data_t *amd = find_amiibo_data(head, tail);
    if (amd != NULL) {
        mui_canvas_draw_utf8(p_canvas, 0, y += 12, amd->name);
        mui_canvas_draw_utf8(p_canvas, 0, y += 12, amd->game_series);
        mui_canvas_draw_utf8(p_canvas, 0, y += 12, amd->notes);
    } else {
        mui_canvas_draw_utf8(p_canvas, 0, y += 12, "空标签");
    }
}

static void amiibo_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    amiibo_view_t *p_amiibo_view = p_view->user_data;
    switch (event->key) {
    case INPUT_KEY_LEFT:
        break;
    case INPUT_KEY_RIGHT:
        break;
    case INPUT_KEY_CENTER:
        if (p_amiibo_view->on_close_cb) {
            p_amiibo_view->on_close_cb(p_amiibo_view);
        }
        break;
    }
}

static void amiibo_view_on_enter(mui_view_t *p_view) {
    amiibo_view_t *p_amiibo_view = p_view->user_data;
    ntag_emu_set_tag(p_amiibo_view->amiibo);
    ntag_emu_set_update_cb(amiibo_update_cb, p_amiibo_view);
}

static void amiibo_view_on_exit(mui_view_t *p_view) { ntag_emu_set_update_cb(NULL, NULL); }

amiibo_view_t *amiibo_view_create() {
    amiibo_view_t *p_amiibo_view = mui_mem_malloc(sizeof(amiibo_view_t));
    memset(p_amiibo_view, 0, sizeof(amiibo_view_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_amiibo_view;
    p_view->draw_cb = amiibo_view_on_draw;
    p_view->input_cb = amiibo_view_on_input;
    p_view->enter_cb = amiibo_view_on_enter;
    p_view->exit_cb = amiibo_view_on_exit;

    p_amiibo_view->p_view = p_view;

    return p_amiibo_view;
}

void amiibo_view_free(amiibo_view_t *p_view) {
    mui_view_free(p_view->p_view);
    mui_mem_free(p_view);
}

mui_view_t *amiibo_view_get_view(amiibo_view_t *p_view) { return p_view->p_view; }