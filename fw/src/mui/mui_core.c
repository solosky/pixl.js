#include "mui_core.h"
#include "mui_u8g2.h"
#include "mui_defines.h"



static void mui_process_redraw(mui_t* p_mui, mui_event_t *p_event){

}

static void mui_process_input(mui_t* p_mui, mui_event_t *p_event){

}

static void mui_process_event(void* p_context, mui_event_t *p_event){
    mui_t* p_mui = p_context;
    switch(p_event->id){
        case MUI_EVENT_ID_REDRAW:
        mui_process_redraw(p_mui, p_event);
        break;

        case MUI_EVENT_ID_INPUT:
        mui_process_input(p_mui, p_event);
        break;
    }
}

void mui_init(mui_t* p_mui){
    
    mui_u8g2_init(&p_mui->u8g2);

    mui_event_set_callback(&p_mui->event_queue, mui_process_event, p_mui);
    mui_event_queue_init(&p_mui->event_queue);

    mui_input_init();
}

void mui_post(mui_t* p_mui, mui_event_t*p_event){
    mui_event_post(&p_mui->event_queue, p_event);
}


void mui_tick(mui_t* p_mui){
    mui_event_dispatch(&p_mui->event_queue);
}
