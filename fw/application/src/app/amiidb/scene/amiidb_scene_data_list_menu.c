#include "amiibo_helper.h"
#include "amiidb_scene.h"
#include "app_amiidb.h"
#include "app_error.h"
#include "cwalk2.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "settings.h"
#include "vfs.h"

#include "mui_icons.h"
#include "ntag_emu.h"
#include "ntag_store.h"

#include "db_header.h"
#include "i18n/language.h"
#include "mini_app_launcher.h"
#include "mini_app_registry.h"

static void amiidb_scene_data_list_menu_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                    mui_list_item_t *p_item) {
    app_amiidb_t *app = p_list_view->user_data;
    switch (p_item->icon) {
    case ICON_EXIT:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;

    case ICON_DELETE: {
        char path[VFS_MAX_PATH_LEN];
        sprintf(path, "/amiibo/data/%02d.bin", app->cur_slot_index);
        vfs_driver_t *p_vfs_driver = vfs_get_driver(VFS_DRIVE_EXT);
        p_vfs_driver->remove_file(path);
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    } break;
    }
}

void amiidb_scene_data_list_menu_on_enter(void *user_data) {
    app_amiidb_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
    mui_list_view_add_item(app->p_list_view, ICON_DELETE, "重置..", (void *)0);
    mui_list_view_add_item(app->p_list_view, ICON_EXIT, "[返回]", (void *)0);

    mui_list_view_set_selected_cb(app->p_list_view, amiidb_scene_data_list_menu_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_LIST);
}

void amiidb_scene_data_list_menu_on_exit(void *user_data) {
    app_amiidb_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}
