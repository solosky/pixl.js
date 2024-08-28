#include "app_chameleon.h"
#include "chameleon_scene.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "amiibo_helper.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "i18n/language.h"
#include "settings2.h"

#include "mui_icons.h"
#include "tag_helper.h"

#include "fds_utils.h"

typedef enum {
    CHAMELEON_MENU_BACK,
    CHAMELEON_MENU_FILE,
} chameleon_menu_item_t;

void chameleon_scene_menu_card_data_file_load_from_file(app_chameleon_t *app, const char *file_name) {
    char path[VFS_MAX_PATH_LEN];
    vfs_file_t fd;
    vfs_obj_t obj;
    int32_t err;
    int32_t ret;

    sprintf(path, "%s/%s", CHELEMEON_DUMP_FOLDER, file_name);
    vfs_driver_t *p_driver = vfs_get_default_driver();

    tag_specific_type_t tag_type = tag_helper_get_active_tag_type();
    uint8_t *tag_buffer = tag_helper_get_active_tag_memory_data();
    size_t tag_data_size = tag_helper_get_active_tag_data_size();

    err = p_driver->stat_file(path, &obj);
    if (err < 0) {
        mui_toast_view_show(app->p_toast_view, _T(APP_CHAMELEON_CARD_DATA_LOAD_NOT_FOUND));
        return;
    }

    // TODO file size check
    if (tag_data_size != obj.size) {
        mui_toast_view_show(app->p_toast_view, _T(APP_CHAMELEON_CARD_DATA_LOAD_SIZE_NOT_MATCH));
        return;
    }

    // load file to buffer directly
    err = p_driver->read_file_data(path, tag_buffer, tag_data_size);
    if (err < 0) {
        mui_toast_view_show(app->p_toast_view, _T(APP_CHAMELEON_CARD_DATA_LOAD_FAILED));
        return;
    }

    // set nickname by filename
    err = tag_helper_set_nickname(file_name);
    if (err != 0) {
        mui_toast_view_show(app->p_toast_view, _T(APP_CHAMELEON_CARD_SET_NICK_FAILED));
        return;
    }

    NRF_LOG_INFO("load card data:%d", err);
    mui_toast_view_show(app->p_toast_view, _T(APP_CHAMELEON_CARD_DATA_LOAD_SUCCESS));
    mui_scene_dispatcher_back_scene(app->p_scene_dispatcher, 2);
}

void chameleon_scene_menu_card_data_file_load_reload(app_chameleon_t *app) {
    vfs_dir_t dir;
    vfs_obj_t obj;

    vfs_driver_t *p_driver = vfs_get_default_driver();
    int32_t res = p_driver->open_dir(CHELEMEON_DUMP_FOLDER, &dir);
    if (res < 0) {
        return;
    }

    while (p_driver->read_dir(&dir, &obj) == VFS_OK) {
        if (obj.type == VFS_TYPE_REG) {
            mui_list_view_add_item(app->p_list_view, ICON_FILE, obj.name, (void *)CHAMELEON_MENU_FILE);
        }
    }
    p_driver->close_dir(&dir);
    return;
}

void chameleon_scene_menu_card_data_file_load_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                       mui_list_item_t *p_item) {
    app_chameleon_t *app = p_list_view->user_data;
    chameleon_menu_item_t item = (chameleon_menu_item_t)p_item->user_data;
    switch (item) {
    case CHAMELEON_MENU_FILE: {
        chameleon_scene_menu_card_data_file_load_from_file(app, string_get_cstr(p_item->text));
    } break;

    case CHAMELEON_MENU_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

void chameleon_scene_menu_card_data_file_load_on_enter(void *user_data) {
    app_chameleon_t *app = user_data;

    chameleon_scene_menu_card_data_file_load_reload(app);
    mui_list_view_add_item(app->p_list_view, ICON_BACK, _T(MAIN_RETURN), (void *)CHAMELEON_MENU_BACK);
    mui_list_view_set_selected_cb(app->p_list_view, chameleon_scene_menu_card_data_file_load_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, CHAMELEON_VIEW_ID_LIST);
}

void chameleon_scene_menu_card_data_file_load_on_exit(void *user_data) {
    app_chameleon_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}