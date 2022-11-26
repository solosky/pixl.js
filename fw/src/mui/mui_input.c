#include "mui_input.h"
#include "mui_core.h"
#include "mui_event.h"
#include "nrf_log.h"

static void mui_input_post_event(mui_input_event_t *p_input_event) {
    mui_event_t mui_event = {.arg_int = p_input_event->type << 8 + p_input_event->key};
    mui_post(mui(), &mui_event);
}

void mui_input_init() {}

void mui_input_on_bsp_event(bsp_event_t evt) {
    switch (evt) {

    case BSP_EVENT_KEY_0: {
        NRF_LOG_DEBUG("Key 0 press");
        mui_input_event_t input_event = {.key = INPUT_KEY_LEFT, .type = INPUT_TYPE_SHORT};
        mui_input_post_event(&input_event);
        break;
    }

    case BSP_EVENT_KEY_1: {
        NRF_LOG_DEBUG("Key 1 press");
        mui_input_event_t input_event = {.key = INPUT_KEY_CENTER,
                                         .type = INPUT_TYPE_SHORT};
        mui_input_post_event(&input_event);

        break;
    }

    case BSP_EVENT_KEY_2: {
        NRF_LOG_DEBUG("Key 2 press");
        mui_input_event_t input_event = {.key = INPUT_KEY_RIGHT,
                                         .type = INPUT_TYPE_SHORT};
        mui_input_post_event(&input_event);

        break;
    }
    default:
        break;
    }
}
