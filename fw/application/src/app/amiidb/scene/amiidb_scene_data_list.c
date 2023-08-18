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

#define LINK_MAX_DISPLAY_CNT 100

void amiidb_scene_data_list_reload(app_amiidb_t *app);

static void amiidb_scene_data_list_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                         mui_list_item_t *p_item) {
    uint16_t icon = p_item->icon;
    app_amiidb_t *app = mui_list_view_get_user_data(p_list_view);
    if (event == MUI_LIST_VIEW_EVENT_SELECTED) {
        switch (icon) {
        case ICON_EXIT: {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_MAIN);
        } break;

        case ICON_FOLDER: {
            const db_game_t *p_game = p_item->user_data;
            app->game_id_path[++app->game_id_index] = p_game->game_id;
            amiidb_scene_data_list_reload(app);
        } break;

        case ICON_FILE: {
            const db_amiibo_t *p_amiibo = p_item->user_data;
            app->cur_amiibo = p_amiibo;
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_AMIIBO_DETAIL);
        } break;
        }
    } else if (event == MUI_LIST_VIEW_EVENT_LONG_SELECTED) {
        app->cur_slot_index = mui_list_view_get_focus(p_list_view);
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_DATA_LIST_MENU);
    }
}

void amiidb_scene_data_list_reload(app_amiidb_t *app) {
    settings_data_t *p_settings_data = settings_get_data();
    char txt[64];
    vfs_driver_t *p_vfs_driver;
    vfs_dir_t dir;
    vfs_obj_t obj;
    vfs_meta_t meta;

    // clear list view
    mui_list_view_clear_items(app->p_list_view);

    // add item first
    for (uint8_t i = 0; i < p_settings_data->amiidb_data_slot_num; i++) {
        sprintf(txt, "%02d <空标签>", i + 1);
        mui_list_view_add_item(app->p_list_view, ICON_FILE, txt, (void *)0);
    }

    // list file
    p_vfs_driver = vfs_get_driver(VFS_DRIVE_EXT);
    int32_t res = p_vfs_driver->open_dir("/amiibo/data", &dir);
    if (res == VFS_OK) {
        while (p_vfs_driver->read_dir(&dir, &obj) == VFS_OK) {
            if (obj.type == VFS_TYPE_DIR) {
                // ignore, not supported now
            } else {
                uint8_t index;
                if (sscanf(obj.name, "%02d.bin", &index) == 1) {
                    memset(&meta, 0, sizeof(vfs_meta_t));
                    vfs_meta_decode(obj.meta, sizeof(obj.meta), &meta);
                    if (meta.has_amiibo_id) {
                        const db_amiibo_t *p_amiibo = get_amiibo_by_id(meta.amiibo_head, meta.amiibo_tail);
                        if (p_amiibo != NULL) {
                            const char *name =
                                p_settings_data->language == LANGUAGE_ZH_HANS ? p_amiibo->name_cn : p_amiibo->name_en;
                            sprintf(txt, "%02d %s", index + 1, name);
                        } else {
                            sprintf(txt, "Amiibo[%08x:%08x]", meta.amiibo_head, meta.amiibo_tail);
                        }

                        mui_list_view_set_item(app->p_list_view, index, ICON_AMIIBO, txt, (void *)0);
                    }
                }
            }
        }
        p_vfs_driver->close_dir(&dir);
    }

    mui_list_view_add_item(app->p_list_view, ICON_EXIT, "[返回]", (void *)0);
}

static void amiidb_scene_data_list_setup(app_amiidb_t *app) {
    mui_list_view_set_selected_cb(app->p_list_view, amiidb_scene_data_list_list_view_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_LIST);
}

void amiidb_scene_data_list_on_enter(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    amiidb_scene_data_list_reload(app);
    amiidb_scene_data_list_setup(app);
}

void amiidb_scene_data_list_on_exit(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    mui_list_view_clear_items(app->p_list_view);
}