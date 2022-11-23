#include "mui_event.h"
#include "nrf_log.h"

void mui_event_queue_init(mui_event_queue_t *p_queue) {
    mui_event_array_init(p_queue->event_array);
}
void mui_event_set_callback(mui_event_queue_t *p_queue, mui_event_handler_t dispatcher,
                            void *context) {
    p_queue->dispatch_context = context;
    p_queue->dispatcher = dispatcher;
}

void mui_event_post(mui_event_queue_t *p_queue, mui_event_t *p_event) {
    if (!mui_event_array_size(p_queue->event_array) > MAX_EVENT_MSG) {
        NRF_LOG_WARNING("event buffer is FULL!!");
        return;
    }
    mui_event_t* p_new = mui_event_array_push_new(p_queue->event_array);
    memcpy(p_new, p_event, sizeof(mui_event_t));
    
}

void mui_event_dispatch(mui_event_queue_t *p_queue) {
    mui_event_t event;
    //TODO  ..
    // while (mui_event_buffer_pop(&p_queue->event_buffer, &event)) {
    //     p_queue->dispatcher(p_queue->dispatch_context, &event);
    // }
}