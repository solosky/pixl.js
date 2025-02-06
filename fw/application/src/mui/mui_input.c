#include "mui_input.h"

#include "mui_core.h"
#include "mui_event.h"
#include "nrf_log.h"
#include "bsp_btn.h"

#include "cache.h"

void mui_input_post_event(mui_input_event_t *p_input_event) {
    uint32_t arg = p_input_event->type;
    arg <<= 8;
    arg += p_input_event->key;
    mui_event_t mui_event = {.id = MUI_EVENT_ID_INPUT, .arg_int = arg};
    mui_post(mui(), &mui_event);
}


void mui_input_on_bsp_btn_event(uint8_t btn, bsp_btn_event_t evt) {
    switch (evt) {

    case BSP_BTN_EVENT_PRESSED: {
        NRF_LOG_DEBUG("Key %d pressed", btn);
        mui_input_event_t input_event = {.key = btn, .type = INPUT_TYPE_PRESS};
        mui_input_post_event(&input_event);
        break;
    }

    case BSP_BTN_EVENT_RELEASED: {
        NRF_LOG_DEBUG("Key %d released", btn);
        mui_input_event_t input_event = {.key = btn, .type = INPUT_TYPE_RELEASE};
        mui_input_post_event(&input_event);
        break;
    }

    case BSP_BTN_EVENT_SHORT: {
        NRF_LOG_DEBUG("Key %d short push", btn);
        mui_input_event_t input_event = {.key = btn,
                                         .type = INPUT_TYPE_SHORT};
        mui_input_post_event(&input_event);
        break;
    }

    case BSP_BTN_EVENT_LONG: {
        NRF_LOG_DEBUG("Key %d long push", btn);
        mui_input_event_t input_event = {.key = btn,
                                         .type = INPUT_TYPE_LONG};
        mui_input_post_event(&input_event);

        break;
    }

     case BSP_BTN_EVENT_REPEAT: {
        NRF_LOG_DEBUG("Key %d repeat push", btn);
        mui_input_event_t input_event = {.key = btn,
                                         .type = INPUT_TYPE_REPEAT};
        mui_input_post_event(&input_event);

        break;
    }

    default:
        break;
    }
}


void mui_input_init() {
    bsp_btn_init(mui_input_on_bsp_btn_event);
    
}
