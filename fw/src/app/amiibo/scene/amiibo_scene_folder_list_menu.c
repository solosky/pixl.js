#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "vos.h"

enum folder_list_menu_t {
    FOLDER_LIST_MENU_CREATE_FOLDER,
    FOLDER_LIST_MENU_REMOVE_FOLDER,
    FOLDER_LIST_MENU_EMPTY_FOLDER,
    FOLDER_LIST_MENU_BACK,
};

static void amiibo_scene_folder_list_menu_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                      mui_list_item_t *p_item) {
    app_amiibo_t *app = p_list_view->user_data;
    uint32_t index = (uint32_t)p_item->user_data;
    vos_driver_t *p_driver = vos_get_driver(app->current_drive);
    switch (index) {
    case FOLDER_LIST_MENU_CREATE_FOLDER: {
        // TODO INPUT
        int32_t res = p_driver->create_folder(VOS_BUCKET_AMIIBO, "荒野之息");
        if (res == VOS_OK) {
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }
        break;
    }

    case FOLDER_LIST_MENU_REMOVE_FOLDER: {
        int32_t res = p_driver->remove_folder(VOS_BUCKET_AMIIBO, string_get_cstr(app->current_folder));
        if (res == VOS_OK) {
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }
    } break;

    case FOLDER_LIST_MENU_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

void amiibo_scene_folder_list_menu_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;

    mui_list_view_add_item(app->p_list_view, 0xe1c8, "新建文件夹", (void *)FOLDER_LIST_MENU_CREATE_FOLDER);
    mui_list_view_add_item(app->p_list_view, 0xe1c9, "删除文件夹", (void *)FOLDER_LIST_MENU_REMOVE_FOLDER);
    mui_list_view_add_item(app->p_list_view, 0xe1ca, "清空文件夹", (void *)FOLDER_LIST_MENU_EMPTY_FOLDER);
    mui_list_view_add_item(app->p_list_view, 0xe069, "返回列表", (void *)FOLDER_LIST_MENU_BACK);

    mui_list_view_set_selected_cb(app->p_list_view, amiibo_scene_folder_list_menu_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
}

void amiibo_scene_folder_list_menu_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}