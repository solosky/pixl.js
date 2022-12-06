#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "mui_list_view.h"

static void amiibo_scene_storage_list_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                  mui_list_item_t *p_item) {
    app_amiibo_t *p_app = p_list_view->user_data;
    mui_scene_dispatcher_next_scene(p_app->p_scene_dispatcher, AMIIBO_SCENE_FOLDER_LIST);
}

void amiibo_scene_storage_list_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_add_item(app->p_list_view, 0xe1bb, "Internal Flash", 0);
    mui_list_view_add_item(app->p_list_view, 0xe1bb, "External Flash", 1);

    mui_list_view_set_selected_cb(app->p_list_view, amiibo_scene_storage_list_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
}

void amiibo_scene_storage_list_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}