#ifndef MUI_EVENT_H
#define MUI_EVENT_H

#include <stdint.h>

typedef struct {
    uint32_t id;
    uint32_t arg_int;
    void*  arg_ptr;
} mui_event_t;

typedef void (*mui_event_handler_t)(void* context, mui_event_t *p_event);

typedef struct {
    mui_event_handler_t dispatcher;
    void* dispatch_context;
} mui_event_queue_t;








void mui_event_queue_init(mui_event_queue_t *p_queue);
void mui_event_set_callback(mui_event_queue_t *p_queue, mui_event_handler_t dispatcher, void * context);
void mui_event_post(mui_event_queue_t *p_queue, mui_event_t *p_event);
void mui_event_dispatch(mui_event_queue_t *p_queue);


#endif 