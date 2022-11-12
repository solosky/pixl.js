#include "mui_event.h"
#include "nrf_log.h"

void mui_event_queue_init(mui_event_queue_t *p_queue){
    mui_event_buffer_init(&p_queue->event_buffer);
}
void mui_event_set_callback(mui_event_queue_t *p_queue, mui_event_handler_t dispatcher, void * context){
    p_queue->dispatch_context = context;
    p_queue->dispatcher = dispatcher;

}
void mui_event_post(mui_event_queue_t *p_queue, mui_event_t *p_event){
    if(!mui_event_buffer_push(&p_queue->event_buffer, *p_event)){
        NRF_LOG_WARNING("event buffer is FULL!!");
    }
}

void mui_event_dispatch(mui_event_queue_t *p_queue){
    mui_event_t event;
    while(mui_event_buffer_pop(&p_queue->event_buffer, &event)){
        p_queue->dispatcher(p_queue->dispatch_context, &p_event);
    }
}