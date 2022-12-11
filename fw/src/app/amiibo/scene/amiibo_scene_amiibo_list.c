#include "amiibo_data.h"
#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "cwalk.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "vos.h"
#include <string.h>

static void amiibo_scene_amiibo_list_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                 mui_list_item_t *p_item) {
    app_amiibo_t *app = p_list_view->user_data;
    uint32_t idx = (uint32_t)p_item->user_data;
    string_set(app->current_file, p_item->text);

    vos_driver_t *p_driver = vos_get_driver(app->current_drive);

    if (event == MUI_LIST_VIEW_EVENT_SELECTED) {
        if (idx == 0xFFFF) {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FOLDER_LIST);
        } else {
            // read tag
            int32_t res = p_driver->read_object(VOS_BUCKET_AMIIBO, string_get_cstr(app->current_folder),
                                                string_get_cstr(app->current_file), &app->ntag, sizeof(ntag_t));
            if (res > 0) {
                mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_AMIIBO_DETAIL);
            }
        }
    } else {
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_AMIIBO_LIST_MENU);
    }
}

void amiibo_scene_amiibo_list_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;
    ntag_t ntag;
    mui_list_view_add_item(app->p_list_view, 0xe1d6, "..", (void *)0xFFFF);

    vos_driver_t *p_driver = vos_get_driver(app->current_drive);

    vos_obj_t files[VOS_MAX_OBJECT_SIZE];
    int32_t file_size =
        p_driver->list_object(VOS_BUCKET_AMIIBO, string_get_cstr(app->current_folder), files, VOS_MAX_OBJECT_SIZE);

    for (uint32_t i = 0; i < file_size; i++) {
        mui_list_view_add_item(app->p_list_view, 0xe1d6, files[i].name, (void *)i);
    }

    mui_list_view_set_selected_cb(app->p_list_view, amiibo_scene_amiibo_list_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
}

void amiibo_scene_amiibo_list_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}