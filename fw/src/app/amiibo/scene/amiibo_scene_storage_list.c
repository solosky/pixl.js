#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "mui_list_view.h"
#include "vos.h"

static void amiibo_scene_storage_list_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                  mui_list_item_t *p_item) {
    app_amiibo_t *p_app = p_list_view->user_data;
    p_app->current_drive = (vos_drive_t)p_item->user_data;
    vos_driver_t *p_driver = vos_get_driver(p_app->current_drive);
    if (p_driver->mounted()) {
        mui_scene_dispatcher_next_scene(p_app->p_scene_dispatcher, AMIIBO_SCENE_FOLDER_LIST);
    } else {
        int32_t err = p_driver->mount();
        if (err == VOS_OK) {
            mui_scene_dispatcher_next_scene(p_app->p_scene_dispatcher, AMIIBO_SCENE_FOLDER_LIST);
        }
    }
}

void amiibo_scene_storage_list_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;

    if (vos_drive_enabled(VOS_DRIVE_INT)) {
        mui_list_view_add_item(app->p_list_view, 0xe1bb, "Internal Flash", (void*) VOS_DRIVE_INT);
    }
    if (vos_drive_enabled(VOS_DRIVE_EXT)) {
        mui_list_view_add_item(app->p_list_view, 0xe1bb, "External Flash", (void*) VOS_DRIVE_EXT);
    }

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