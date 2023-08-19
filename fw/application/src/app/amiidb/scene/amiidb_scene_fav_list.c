#include "amiibo_helper.h"
#include "amiidb_scene.h"
#include "app_amiidb.h"
#include "mui_list_view.h"
#include "nrf_log.h"

#include "db_header.h"
#include "mui_icons.h"
#include "settings.h"
#include "vfs.h"
#include <math.h>

#define LINK_MAX_DISPLAY_CNT 100

typedef struct {
    uint8_t game_id;
    const db_amiibo_t *p_amiibo;
    string_t file_name;
} fav_item_t;

static void amiidb_scene_fav_list_reload(app_amiidb_t *app);

static void amiidb_scene_fav_list_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                        mui_list_item_t *p_item) {
    uint16_t icon = p_item->icon;
    app_amiidb_t *app = mui_list_view_get_user_data(p_list_view);
    if (event == MUI_LIST_VIEW_EVENT_SELECTED) {
        switch (icon) {
        case ICON_EXIT:
            if (string_size(app->cur_fav_dir) == 0) {
                mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_MAIN);
            } else {
                string_reset(app->cur_fav_dir);
                amiidb_scene_fav_list_reload(app);
            }
            break;

        case ICON_FILE: {
            fav_item_t *p_fav_item = p_item->user_data;
            app->cur_amiibo = p_fav_item->p_amiibo;
            app->prev_scene_id = AMIIDB_SCENE_FAV_LIST;
            string_set(app->cur_fav_file, p_item->text);
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_AMIIBO_DETAIL);
        } break;

        case ICON_FOLDER: {
            string_set(app->cur_fav_dir, p_item->text);
            amiidb_scene_fav_list_reload(app);
        } break;
        }
    } else {
        if (icon == ICON_FILE) {
            fav_item_t *p_fav_item = p_item->user_data;
            string_set(app->cur_fav_file, p_fav_item->file_name);
        } else if (icon == ICON_FOLDER) {
            string_set(app->cur_fav_file, p_item->text);
        }

        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_FAV_LIST_MENU);
    }
}

static int amiidb_scene_fav_list_list_view_sort_cb(const mui_list_item_t *p_item_a, const mui_list_item_t *p_item_b) {
    if (p_item_a->icon == ICON_FOLDER && p_item_b->icon == ICON_FOLDER) {
        db_game_t *p_game_a = (db_game_t *)p_item_a->user_data;
        db_game_t *p_game_b = (db_game_t *)p_item_b->user_data;
        return p_game_b->order - p_game_a->order;
    } else if (p_item_a->icon == ICON_FOLDER && p_item_b->icon == ICON_FILE) {
        return -1;
    } else {
        db_amiibo_t *p_amiibo_a = (db_amiibo_t *)p_item_a->user_data;
        db_amiibo_t *p_amiibo_b = (db_amiibo_t *)p_item_b->user_data;
        return strcmp(p_amiibo_a->name_en, p_amiibo_b->name_en);
    }
}

static void amiidb_scene_fav_list_reload(app_amiidb_t *app) {
    settings_data_t *p_settings_data = settings_get_data();
    char txt[64];
    vfs_driver_t *p_vfs_driver;
    vfs_dir_t dir;
    vfs_obj_t obj;

    // clear list view
    mui_list_view_clear_items(app->p_list_view);
    p_vfs_driver = vfs_get_driver(VFS_DRIVE_EXT);

    strcpy(txt, "/amiibo/fav");
    if (string_size(app->cur_fav_dir) > 0) {
        strcat(txt, "/");
        strcat(txt, string_get_cstr(app->cur_fav_dir));
    }
    int32_t res = p_vfs_driver->open_dir(txt, &dir);
    if (res == VFS_OK) {
        while (p_vfs_driver->read_dir(&dir, &obj) == VFS_OK) {
            if (obj.type == VFS_TYPE_DIR) {
                mui_list_view_add_item(app->p_list_view, ICON_FOLDER, obj.name, (void *)-1);
            } else {
                uint8_t game_id;
                uint32_t head, tail;

                if (sscanf(obj.name, "%d_%08X_%08X.fav", &game_id, &head, &tail) == 3) {
                    const db_amiibo_t *p_amiibo = get_amiibo_by_id(head, tail);

                    fav_item_t *p_item = mui_mem_malloc(sizeof(fav_item_t));
                    p_item->p_amiibo = p_amiibo;
                    p_item->game_id = game_id;
                    string_init(p_item->file_name);
                    m_string_set_cstr(p_item->file_name, obj.name);

                    if (p_amiibo) {
                        const char *name =
                            p_settings_data->language == LANGUAGE_ZH_HANS ? p_amiibo->name_cn : p_amiibo->name_en;
                        mui_list_view_add_item(app->p_list_view, ICON_FILE, name, p_item);
                    } else {
                        sprintf(txt, "Amiibo[%08x:%08x]", head, tail);
                        mui_list_view_add_item(app->p_list_view, ICON_FILE, obj.name, p_item);
                    }
                }
            }
        }
        p_vfs_driver->close_dir(&dir);
    }

    mui_list_view_add_item(app->p_list_view, ICON_EXIT, "[返回]", (void *)0);
    mui_list_view_set_selected_cb(app->p_list_view, amiidb_scene_fav_list_list_view_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_LIST);
}

void amiidb_scene_fav_list_item_clear_cb(mui_list_item_t *p_item) {
    if (p_item->icon == ICON_FILE) {
        fav_item_t *p_fav_item = (fav_item_t *)p_item->user_data;
        string_clear(p_fav_item->file_name);
        mui_mem_free(p_fav_item);
    }
}

void amiidb_scene_fav_list_on_enter(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    amiidb_scene_fav_list_reload(app);
}

void amiidb_scene_fav_list_on_exit(void *user_data) {
    app_amiidb_t *app = (app_amiidb_t *)user_data;
    mui_list_view_clear_items_with_cb(app->p_list_view, amiidb_scene_fav_list_item_clear_cb);
}