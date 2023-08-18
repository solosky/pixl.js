#include "amiibo_helper.h"
#include "amiidb_scene.h"
#include "app_amiidb.h"
#include "mui_list_view.h"
#include "nrf_log.h"

#include "db_header.h"
#include "mui_icons.h"
#include "settings.h"
#include "vfs.h"
#include "vfs_meta.h"

#include <math.h>

void amiidb_scene_data_list_reload(app_amiidb_t *app);

static void amiidb_scene_data_select_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                           mui_list_item_t *p_item) {
    uint16_t icon = p_item->icon;
    app_amiidb_t *app = mui_list_view_get_user_data(p_list_view);
    switch (icon) {
    case ICON_EXIT: {
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    } break;

    case ICON_AMIIBO:
    case ICON_FILE: {
        char path[VFS_MAX_PATH_LEN];
        vfs_meta_t meta;
        uint8_t meta_encoded[VFS_META_MAX_SIZE];
        const db_amiibo_t *p_amiibo = app->cur_amiibo;

        uint32_t index = mui_list_view_get_focus(p_list_view);
        sprintf(path, "/amiibo/data/%02d.bin", index);
        vfs_driver_t *p_driver = vfs_get_driver(VFS_DRIVE_EXT);
        int res = p_driver->write_file_data(path, app->ntag.data, sizeof(app->ntag.data));
        if (res < 0) {
            //TODO msg box
            return;
        }
        meta.has_amiibo_id = true;
        meta.amiibo_head = p_amiibo->head;
        meta.amiibo_tail = p_amiibo->tail;

        vfs_meta_encode(meta_encoded, sizeof(meta_encoded), &meta);
        res = p_driver->update_file_meta(path, meta_encoded, sizeof(meta_encoded));
        if(res < 0){
            //TODO msg box
            return;
        }

        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_AMIIBO_DETAIL);
    } break;
    }
}

static void amiidb_scene_data_select_setup(app_amiidb_t *app) {
    mui_list_view_set_selected_cb(app->p_list_view, amiidb_scene_data_select_list_view_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_LIST);
}


void amiidb_scene_data_select_on_enter(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    amiidb_scene_data_list_reload(app);
    amiidb_scene_data_select_setup(app);
}

void amiidb_scene_data_select_on_exit(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    mui_list_view_clear_items(app->p_list_view);
}