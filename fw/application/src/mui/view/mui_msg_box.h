#ifndef MUI_MSG_BOX_H
#define MUI_MSG_BOX_H

#include "mui_include.h"

struct mui_msg_box_s;
typedef struct mui_msg_box_s mui_msg_box_t;

typedef enum {
    MUI_MSG_BOX_EVENT_SELECT_LEFT,
    MUI_MSG_BOX_EVENT_SELECT_CENTER,
    MUI_MSG_BOX_EVENT_SELECT_RIGHT,
} mui_msg_box_event_t;

typedef void (*mui_msg_box_event_cb_t)(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box);

struct mui_msg_box_s {
    mui_view_t *p_view;
    string_t header;
    string_t message;
    string_t btn_left_text;
    string_t btn_center_text;
    string_t btn_right_text;
    uint8_t focus;
    mui_msg_box_event_cb_t event_cb;
    void *user_data;
};

mui_msg_box_t *mui_msg_box_create();
void mui_msg_box_free(mui_msg_box_t *p_view);
mui_view_t *mui_msg_box_get_view(mui_msg_box_t *p_view);
void mui_msg_box_set_user_data(mui_msg_box_t *p_view, void *user_data);
void* mui_msg_box_get_user_data(mui_msg_box_t *p_view);

//// view functions //
void mui_msg_box_set_event_cb(mui_msg_box_t *p_view, mui_msg_box_event_cb_t event_cb);
void mui_msg_box_set_header(mui_msg_box_t *p_view, const char* header);
void mui_msg_box_set_message(mui_msg_box_t *p_view, const char* message);
void mui_msg_box_set_btn_text(mui_msg_box_t *p_view, const char* left, const char* center, const char* right);
void mui_msg_box_set_btn_focus(mui_msg_box_t *p_view, uint8_t focus);
void mui_msg_box_reset(mui_msg_box_t *p_view);

#endif