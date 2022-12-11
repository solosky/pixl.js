#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "ntag_def.h"
#include "vos.h"

enum amiibo_list_menu_t {
    AMIIBO_LIST_MENU_CREATE_AMIIBO,
    AMIIBO_LIST_MENU_REMOVE_AMIIBO,
    AMIIBO_LIST_MENU_BACK,
};

static void amiibo_scene_amiibo_list_menu_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                      mui_list_item_t *p_item) {
    app_amiibo_t *app = p_list_view->user_data;
    uint32_t index = (uint32_t)p_item->user_data;
    vos_driver_t *p_driver = vos_get_driver(app->current_drive);
    const char *file = string_get_cstr(app->current_file);
    const char *folder = string_get_cstr(app->current_folder);
    switch (index) {
    case AMIIBO_LIST_MENU_CREATE_AMIIBO: {
        extern const ntag_t default_ntag215;
        ntag_t tmp_ntag;
        memcpy(&tmp_ntag, &default_ntag215, sizeof(ntag_t));

        int32_t res = p_driver->write_object(VOS_BUCKET_AMIIBO, folder, "001.bin", &tmp_ntag, sizeof(tmp_ntag));
        if (res > 0) {
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }

        break;
    }

    case AMIIBO_LIST_MENU_REMOVE_AMIIBO: {
        if (strcmp(file, "..") != 0) {
            p_driver->remove_object(VOS_BUCKET_AMIIBO, folder, file);
        }
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }

    case AMIIBO_LIST_MENU_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

void amiibo_scene_amiibo_list_menu_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;

    mui_list_view_add_item(app->p_list_view, 0xe1c8, "新建amiibo", (void *)AMIIBO_LIST_MENU_CREATE_AMIIBO);
    mui_list_view_add_item(app->p_list_view, 0xe1c9, "删除amiibo", (void *)AMIIBO_LIST_MENU_REMOVE_AMIIBO);
    mui_list_view_add_item(app->p_list_view, 0xe069, "返回列表", (void *)AMIIBO_LIST_MENU_BACK);

    mui_list_view_set_selected_cb(app->p_list_view, amiibo_scene_amiibo_list_menu_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
}

void amiibo_scene_amiibo_list_menu_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}