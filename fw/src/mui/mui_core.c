#include "mui_core.h"

#include "mui_defines.h"
#include "mui_u8g2.h"

static mui_view_port_t *mui_find_view_port_enabled(mui_t *p_mui, mui_layer_t layer) {
    mui_view_port_array_it_t it;
    mui_view_port_array_it_last(it, p_mui->layers[layer]);
    while (!mui_view_port_array_end_p(it)) {
        mui_view_port_t *p_view_port = *mui_view_port_array_ref(it);
        if (p_view_port->enabled) {
            return p_view_port;
        }
        mui_view_port_array_previous(it);
    }
    return NULL;
}

static void mui_process_redraw(mui_t *p_mui, mui_event_t *p_event) {
    mui_view_port_t *p_view_port =
        mui_find_view_port_enabled(p_mui, MUI_LAYER_FULLSCREEN);
    if (p_view_port) {
        mui_canvas_clear(&p_mui->canvas);
        mui_canvas_set_frame(&p_mui->canvas, 0, 0, p_mui->screen_width,
                             p_mui->screen_height);
        p_view_port->draw_cb(p_view_port, &p_mui->canvas);
        mui_canvas_flush(&p_mui->canvas);
        return;
    }

    p_view_port = mui_find_view_port_enabled(p_mui, MUI_LAYER_WINDOW);
    if (p_view_port) {
        mui_canvas_clear(&p_mui->canvas);
        mui_canvas_set_frame(&p_mui->canvas, 0, LAYER_STATUS_BAR_HEIGHT,
                             p_mui->screen_width,
                             p_mui->screen_height - LAYER_STATUS_BAR_HEIGHT);
        p_view_port->draw_cb(p_view_port, &p_mui->canvas);
        mui_view_port_t *p_view_port_status_bar =
            mui_find_view_port_enabled(p_mui, MUI_LAYER_STATUS_BAR);
        if (p_view_port_status_bar) {
            mui_canvas_set_frame(&p_mui->canvas, 0, 0, p_mui->screen_width,
                                 LAYER_STATUS_BAR_HEIGHT);
            p_view_port_status_bar->draw_cb(p_view_port_status_bar, &p_mui->canvas);
        }
        mui_canvas_flush(&p_mui->canvas);
        return;
    }

    p_view_port = mui_find_view_port_enabled(p_mui, MUI_LAYER_DESKTOP);
    if (p_view_port) {
        mui_canvas_clear(&p_mui->canvas);
        mui_canvas_set_frame(&p_mui->canvas, 0, LAYER_STATUS_BAR_HEIGHT,
                             p_mui->screen_width,
                             p_mui->screen_height - LAYER_STATUS_BAR_HEIGHT);
        p_view_port->draw_cb(p_view_port, &p_mui->canvas);
        mui_view_port_t *p_view_port_status_bar =
            mui_find_view_port_enabled(p_mui, MUI_LAYER_STATUS_BAR);
        if (p_view_port_status_bar) {
            mui_canvas_set_frame(&p_mui->canvas, 0, 0, p_mui->screen_width,
                                 LAYER_STATUS_BAR_HEIGHT);
            p_view_port_status_bar->draw_cb(p_view_port_status_bar, &p_mui->canvas);
        }
        mui_canvas_flush(&p_mui->canvas);
        return;
    }
}

static void mui_process_input(mui_t *p_mui, mui_event_t *p_event) {
    mui_view_port_t *p_view_port =
        mui_find_view_port_enabled(p_mui, MUI_LAYER_FULLSCREEN);
    if (!p_view_port) {
        p_view_port = mui_find_view_port_enabled(p_mui, MUI_LAYER_WINDOW);
    }
    if (!p_view_port) {
        p_view_port = mui_find_view_port_enabled(p_mui, MUI_LAYER_DESKTOP);
    }
    if (p_view_port) {
        mui_input_event_t input_event;
        uint32_t arg = p_event->arg_int;

        input_event.key = arg & 0xFF;
        input_event.type = (arg >> 8) & 0xFF;

        p_view_port->input_cb(p_view_port, &input_event);

        mui_update(mui());
    }
}

static void mui_process_event(void *p_context, mui_event_t *p_event) {
    mui_t *p_mui = p_context;
    switch (p_event->id) {
    case MUI_EVENT_ID_REDRAW:
        mui_process_redraw(p_mui, p_event);
        break;

    case MUI_EVENT_ID_INPUT:
        mui_process_input(p_mui, p_event);
        break;
    }
}

mui_t *mui() {
    static mui_t mui;
    return &mui;
}

void mui_init(mui_t *p_mui) {
    mui_u8g2_init(&p_mui->u8g2);

    p_mui->screen_height = SCREEN_HEIGHT;
    p_mui->screen_width = SCREEN_WIDTH;
    p_mui->canvas.fb = &p_mui->u8g2;
    p_mui->canvas.height = p_mui->screen_height;
    p_mui->canvas.width = p_mui->screen_width;
    p_mui->canvas.offset_x = 0;
    p_mui->canvas.offset_y = 0;

    for (size_t i = 0; i < MUI_LAYER_MAX; i++) {
        mui_view_port_array_init(p_mui->layers[i]);
    }

    mui_event_set_callback(&p_mui->event_queue, mui_process_event, p_mui);
    mui_event_queue_init(&p_mui->event_queue);

    mui_input_init();

    p_mui->initialized = true;
}

void mui_deinit(mui_t *p_mui) {
    mui_u8g2_deinit(&p_mui->u8g2);
    p_mui->initialized = false;
}

void mui_post(mui_t *p_mui, mui_event_t *p_event) {
    mui_event_post(&p_mui->event_queue, p_event);
}

void mui_tick(mui_t *p_mui) { mui_event_dispatch(&p_mui->event_queue); }

void mui_panic(mui_t *p_mui, char *err) {
    if (p_mui->initialized) {
        u8g2_ClearBuffer(&p_mui->u8g2);
        u8g2_SetFont(&p_mui->u8g2, u8g2_font_wqy12_t_gb2312a);
        u8g2_DrawBox(&p_mui->u8g2, 0, 0, 128, 12);
        u8g2_SetDrawColor(&p_mui->u8g2, 0);
        u8g2_DrawUTF8(&p_mui->u8g2, 28, 10, "SYSTEM FAULT");

        u8g2_SetDrawColor(&p_mui->u8g2, 1);

        uint8_t x = 0;
        uint8_t y = 24;
        uint32_t i = 0;
        uint8_t m = u8g2_GetMaxCharWidth(&p_mui->u8g2);

        while (err[i] != 0 && y < 64) {
            uint8_t w = u8g2_DrawGlyph(&p_mui->u8g2, x, y, err[i]);
            x += w;
            if (x > 128 - m) {
                x = 0;
                y += 12;
            }
            i++;
        }

        u8g2_SendBuffer(&p_mui->u8g2);
    }
}

void mui_add_view_port(mui_t *p_mui, mui_view_port_t *p_vp, mui_layer_t layer) {
    // verify that view port is not added
    mui_view_port_array_it_t it;
    for (size_t i = 0; i < MUI_LAYER_MAX; i++) {
        mui_view_port_array_it(it, p_mui->layers[layer]);
        while (!mui_view_port_array_end_p(it)) {
            mui_assert(*mui_view_port_array_ref(it) != p_vp);
            mui_view_port_array_next(it);
        }
    }

    mui_view_port_array_push_back(p_mui->layers[layer], p_vp);
    mui_update(p_mui);
}
void mui_remove_view_port(mui_t *p_mui, mui_view_port_t *p_vp, mui_layer_t layer) {
    mui_view_port_array_it_t it;
    for (size_t i = 0; i < MUI_LAYER_MAX; i++) {
        mui_view_port_array_it(it, p_mui->layers[i]);
        while (!mui_view_port_array_end_p(it)) {
            if (*mui_view_port_array_ref(it) == p_vp) {
                mui_view_port_array_remove(p_mui->layers[i], it);
            } else {
                mui_view_port_array_next(it);
            }
        }
    }
}

void mui_update(mui_t *p_mui) {
    mui_event_t event = {.id = MUI_EVENT_ID_REDRAW};
    mui_post(p_mui, &event);
}
