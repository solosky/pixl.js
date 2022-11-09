#ifndef MUI_EVENT_H
#define MUI_EVENT_H


typedef struct {
    //TODO 
} mui_event_t;

typedef struct {


} mui_event_queue_t;

#endif 



void mui_event_queue_init(mui_event_queue_t *p_queue);
void mui_event_post(mui_event_queue_t *p_queue, mui_event_t *p_event);
void mui_event_dispatch(mui_event_queue_t *p_queue);