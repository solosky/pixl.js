
#include "app_settings.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "vfs.h"

#include "i18n/language.h"

#define ICON_HOME 0xe1f0

enum storage_list_menu_t { STORAGE_LIST_MENU_MOUNT, STORAGE_LIST_MENU_FORMAT, STORAGE_LIST_MENU_BACK };

static void settings_scene_storage_msg_box_format_cb(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box) {
    app_settings_t *app = p_msg_box->user_data;
    vfs_driver_t *p_driver = vfs_get_default_driver();
    if (event == MUI_MSG_BOX_EVENT_SELECT_LEFT) {
        mui_msg_box_set_message(app->p_msg_box, getLangString(_L_DELETING_MESSAGE));
        mui_msg_box_set_btn_text(app->p_msg_box, NULL, NULL, NULL);

        mui_update_now(mui());

        int32_t res = p_driver->format();
        if (res == VFS_OK) {
            p_driver->mount();
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }
    } else if (event == MUI_MSG_BOX_EVENT_SELECT_RIGHT) {
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    }
}

static void settings_scene_storage_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                               mui_list_item_t *p_item) {
    app_settings_t *app = p_list_view->user_data;
    uint32_t index = (uint32_t)p_item->user_data;
    vfs_driver_t *p_driver = vfs_get_default_driver();
    switch (index) {

    case STORAGE_LIST_MENU_FORMAT: {

        mui_msg_box_set_header(app->p_msg_box, getLangString(_L_FORMAT_STORAGE));
        mui_msg_box_set_message(app->p_msg_box, getLangString(_L_DELETE_ALL_DATA));
        mui_msg_box_set_btn_text(app->p_msg_box, getLangString(_L_CONFIRM), NULL, getLangString(_L_CANCEL));
        mui_msg_box_set_btn_focus(app->p_msg_box, 2);
        mui_msg_box_set_event_cb(app->p_msg_box, settings_scene_storage_msg_box_format_cb);

        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SETTINGS_VIEW_ID_MSG_BOX);
    } break;

    case STORAGE_LIST_MENU_BACK:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

void settings_scene_storage_on_enter(void *user_data) {
    app_settings_t *app = user_data;

    vfs_driver_t *p_driver = vfs_get_default_driver();
    vfs_stat_t stat;
    int32_t res = p_driver->stat(&stat);
    char txt[64];
    if (res == VFS_OK) {
#ifdef VFS_LFS_ENABLE
#define mount_str getLangString(_L_MOUNTED_LFS)
#else
#define mount_str getLangString(_L_MOUNTED_FFS)
#endif
        mui_list_view_add_item(app->p_list_view, 0xe1ca, stat.avaliable ? mount_str : getLangString(_L_NOT_MOUNTED),
                               (void *)-1);
        if (stat.avaliable) {
            snprintf(txt, sizeof(txt), "[%d kB]", stat.total_bytes / 1024);
            mui_list_view_add_item_ext(app->p_list_view, 0xe1cb, getLangString(_L_TOTAL_SPACE), txt, (void *)-1);
            snprintf(txt, sizeof(txt), "[%d kB]", stat.free_bytes / 1024);
            mui_list_view_add_item_ext(app->p_list_view, 0xe1cc, getLangString(_L_AVAILABLE_SPACE), txt, (void *)-1);
        }
    }

    mui_list_view_add_item(app->p_list_view, 0xe1cd, getLangString(_L_FORMAT), (void *)STORAGE_LIST_MENU_FORMAT);

    mui_list_view_add_item(app->p_list_view, 0xe069, getLangString(_L_BACK), (void *)STORAGE_LIST_MENU_BACK);

    mui_list_view_set_selected_cb(app->p_list_view, settings_scene_storage_on_selected);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SETTINGS_VIEW_ID_MAIN);
}

void settings_scene_storage_on_exit(void *user_data) {
    app_settings_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}