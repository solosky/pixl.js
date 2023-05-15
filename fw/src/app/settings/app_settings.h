#ifndef APP_SETTINGS_H
#define APP_ETTINGS_H

#include "mini_app_defines.h"

#include "mini_app_registry.h"

#include "mui_include.h"


#include "mui_list_view.h"
#include "mui_u8g2.h"
#include "utils.h"
#include "version2.h"
#include "mui_progress_bar.h"
typedef struct {
    mui_list_view_t *p_list_view;
    mui_progress_bar_t *p_progress_bar;
    mui_view_dispatcher_t *p_view_dispatcher;
    mui_scene_dispatcher_t *p_scene_dispatcher;
} app_settings_t;

typedef enum { SETTINGS_VIEW_ID_MAIN, SETTINGS_VIEW_ID_PROGRESS_BAR } settings_view_id_t;

extern const mini_app_t app_settings_info;

#endif