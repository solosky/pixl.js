#include "amiibo_data.h"
#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "mui_list_view.h"

static void app_amiibo_view_on_close(amiibo_view_t *p_view) {
    app_amiibo_t *app = p_view->user_data;
    mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_AMIIBO_DETAIL_MENU);
}

void amiibo_scene_amiibo_detail_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;
    app->p_amiibo_view->amiibo = &app->ntag;
    app->p_amiibo_view->focus_index = app->current_file_index;
    app->p_amiibo_view->on_close_cb = app_amiibo_view_on_close;

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_DETAIL);
}

void amiibo_scene_amiibo_detail_on_exit(void *user_data) {}