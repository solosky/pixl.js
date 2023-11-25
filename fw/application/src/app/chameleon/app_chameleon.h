#ifndef APP_CHAMELEON_H
#define APP_CHAMELEON_H

#include "chameleon_view.h"
#include "mini_app_defines.h"
#include "mui_list_view.h"
#include "mui_msg_box.h"
#include "mui_text_input.h"
#include "mui_toast_view.h"

typedef enum {
    APP_CHAMELEON_ID_EDIT_TYPE_NONE,
    APP_CHAMELEON_ID_EDIT_TYPE_UID,
    APP_CHAMELEON_ID_EDIT_TYPE_SAK,
    APP_CHAMELEON_ID_EDIT_TYPE_ATQA,
} app_chameleon_id_edit_type_t;

typedef struct {
    chameleon_view_t *p_chameleon_view;
    mui_list_view_t *p_list_view;
    mui_msg_box_t *p_msg_box;
    mui_view_dispatcher_t *p_view_dispatcher;
    mui_scene_dispatcher_t *p_scene_dispatcher;

    mui_toast_view_t *p_toast_view;
    mui_view_dispatcher_t *p_view_dispatcher_toast;
    mui_text_input_t *p_text_input;

    app_chameleon_id_edit_type_t id_edit_type;

} app_chameleon_t;

typedef enum {
    CHAMELEON_VIEW_ID_LIST,
    CHAMELEON_VIEW_ID_MAIN,
    CHAMELEON_VIEW_ID_MSG_BOX,
    CHAMELEON_VIEW_ID_TEXT_INPUT,
    CHAMELEON_VIEW_ID_TOAST
} app_chameleon_view_id_t;

extern mini_app_t app_chameleon_info;

#endif