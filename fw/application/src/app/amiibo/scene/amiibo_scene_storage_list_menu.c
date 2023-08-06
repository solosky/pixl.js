#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "vfs.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"
#include "i18n/language.h"

#define ICON_HOME 0xe1f0

enum storage_list_menu_t {
    STORAGE_LIST_MENU_MOUNT,
    STORAGE_LIST_MENU_FORMAT,
    STORAGE_LIST_MENU_BACK,
    STORAGE_LIST_MENU_BACK_MAIN_MENU
};

static void amiibo_scene_storage_list_menu_msg_box_format_cb(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box) {
    app_amiibo_t *app = p_msg_box->user_data;
    vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);
    if (event == MUI_MSG_BOX_EVENT_SELECT_LEFT) {
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        int32_t res = p_driver->format();
        if (res == VFS_OK) {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_STORAGE_LIST);
        }
    } else if (event == MUI_MSG_BOX_EVENT_SELECT_RIGHT) {
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    }
}

static void amiibo_scene_storage_list_menu_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                       mui_list_item_t *p_item) {
    app_amiibo_t *app = p_list_view->user_data;
    uint32_t index = (uint32_t)p_item->user_data;
    vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);
    switch (index) {

    case STORAGE_LIST_MENU_FORMAT: {

        mui_msg_box_set_header(app->p_msg_box, getLangString(_L_FORMAT_STORAGE));
        mui_msg_box_set_message(app->p_msg_box, getLangString(_L_DELETE_ALL_DATA));
        mui_msg_box_set_btn_text(app->p_msg_box, getLangString(_L_CONFIRM), NULL, getLangString(_L_CANCEL));
        mui_msg_box_set_btn_focus(app->p_msg_box, 0);
        mui_msg_box_set_event_cb(app->p_msg_box, amiibo_scene_storage_list_menu_msg_box_format_cb);

        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_MSG_BOX);
    } break;

    case STORAGE_LIST_MENU_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;

    case STORAGE_LIST_MENU_BACK_MAIN_MENU:
        mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_AMIIBO);
        break;
    }
}

void amiibo_scene_storage_list_menu_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;

    vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);
    vfs_stat_t stat;
    int32_t res = p_driver->stat(&stat);
    char txt[64];
    if (res == VFS_OK) {
        #ifdef VFS_LFS_ENABLE
        #define mount_str getLangString(_L_MOUNTED_LFS)
        #else
        #define mount_str getLangString(_L_MOUNTED_FFS)
        #endif
        mui_list_view_add_item(app->p_list_view, 0xe1ca,
                               stat.avaliable ? mount_str : getLangString(_L_NOT_MOUNTED), (void *)-1);
        if (stat.avaliable) {
            snprintf(txt, sizeof(txt), "%s: %d kB", getLangString(_L_TOTAL_SPACE), stat.total_bytes / 1024);
            mui_list_view_add_item(app->p_list_view, 0xe1cb, txt, (void *)-1);
            snprintf(txt, sizeof(txt), "%s: %d kB", getLangString(_L_AVAILABLE_SPACE), stat.free_bytes / 1024);
            mui_list_view_add_item(app->p_list_view, 0xe1cc, txt, (void *)-1);
        }
    }

    mui_list_view_add_item(app->p_list_view, 0xe1cd, getLangString(_L_FORMAT), (void *)STORAGE_LIST_MENU_FORMAT);

    mui_list_view_add_item(app->p_list_view, 0xe069, getLangString(_L_BACK_TO_LIST), (void *)STORAGE_LIST_MENU_BACK);
    mui_list_view_add_item(app->p_list_view, ICON_HOME, getLangString(_L_BACK_TO_MAIN_MENU), (void *)STORAGE_LIST_MENU_BACK_MAIN_MENU);

    mui_list_view_set_selected_cb(app->p_list_view, amiibo_scene_storage_list_menu_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
}

void amiibo_scene_storage_list_menu_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}