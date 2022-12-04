#ifndef APP_AMIIBO_H
#define APP_AMIIBO_H

#include "mini_app_defines.h"
#include "ntag_def.h"
#include "mui_scene_dispatcher.h"
#include "amiibo_view.h"
#include "mui_list_view.h"


typedef struct {
    amiibo_view_t *p_amiibo_view;
    mui_list_view_t *p_list_view;
    mui_view_dispatcher_t *p_view_dispatcher;
    mui_scene_dispatcher_t *p_scene_dispatcher;
    ntag_t ntag;
} app_amiibo_t;

typedef enum { AMIIBO_VIEW_ID_LIST, AMIIBO_VIEW_ID_DETAIL } amiibo_view_id_t;

extern const mini_app_t app_amiibo_info;

#endif