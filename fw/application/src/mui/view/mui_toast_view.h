//
// Created by solos on 9/9/2023.
//

#ifndef FW_MUI_TOAST_VIEW_H
#define FW_MUI_TOAST_VIEW_H

#include "mui_view.h"
#include "m-string.h"

typedef struct {
    mui_view_t *p_view;
    void* user_data;
    bool is_visible;
    string_t message;
} mui_toast_view_t;


mui_toast_view_t *mui_toast_view_create();
void mui_toast_view_free(mui_toast_view_t *p_view);
void mui_toast_view_show(mui_toast_view_t *p_view, const char *message);
mui_view_t * mui_toast_view_get_view(mui_toast_view_t *p_view);
static inline void mui_toast_view_set_user_data(mui_toast_view_t *p_view, void *user_data) {
    p_view->user_data = user_data;
}

#endif // FW_MUI_TOAST_VIEW_H
