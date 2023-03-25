#include "app_player.h"
#include "player_scene.h"
#include "player_view.h"
#include "vfs.h"
#include "vfs_meta.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define ICON_FOLDER 0xe1d6
#define ICON_FILE 0xe1ed
#define ICON_BACK 0xe069
#define ICON_ERROR 0xe1bb
#define ICON_HOME 0xe1f0

static void player_scene_file_browser_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                  mui_list_item_t *p_item) {
    app_player_t *app = p_list_view->user_data;
    uint32_t idx = (uint32_t)p_item->user_data;

    if (event == MUI_LIST_VIEW_EVENT_SELECTED) {
        if (p_item->icon == ICON_FILE) {
            string_set_str(app->selected_file, "/player/");
            string_cat(app->selected_file, p_item->text);
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, PLAYER_SCENE_PLAY);
        } else if (p_item->icon == ICON_HOME) {
            mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_PLAYER);
        }

    } else {
    }
}

static int player_scene_file_browser_list_item_cmp(const mui_list_item_t *p_item_a, const mui_list_item_t *p_item_b) {
    if (p_item_a->icon != p_item_b->icon) {
        return p_item_a->icon - p_item_b->icon;
    } else {
        return string_cmp(p_item_a->text, p_item_b->text);
    }
}

static void player_scene_file_browser_reload_folders(app_player_t *app) {
    vfs_driver_t *p_vfs_driver;
    vfs_dir_t dir;
    vfs_obj_t obj;
    vfs_meta_t meta;

    mui_list_view_clear_items(app->p_list_view);

    p_vfs_driver = vfs_get_driver(VFS_DRIVE_EXT);

    mui_list_view_add_item(app->p_list_view, ICON_HOME, ">>主菜单<<", (void *)-1);

    int32_t res = p_vfs_driver->open_dir("/player", &dir);
    uint32_t file_cnt = 0;
    if (res == VFS_OK) {
        while ((res = p_vfs_driver->read_dir(&dir, &obj)) == VFS_OK) {
            // hide file or dir if flagged with hidden
            memset(&meta, 0, sizeof(meta));
            vfs_meta_decode(obj.meta, sizeof(obj.meta), &meta);
            if (meta.has_flags && (meta.flags & VFS_OBJ_FLAG_HIDDEN)) {
                continue;
            }
            uint16_t icon = obj.type == VFS_TYPE_DIR ? ICON_FOLDER : ICON_FILE;
            mui_list_view_add_item(app->p_list_view, icon, obj.name, (void *)-1);
            file_cnt++;
        }
        p_vfs_driver->close_dir(&dir);
    } else {
        mui_list_view_add_item(app->p_list_view, ICON_ERROR, "打开文件夹失败", (void *)-1);
    }

    if (file_cnt == 0) {
        mui_list_view_add_item(app->p_list_view, ICON_ERROR, "<无动画文件>", (void *)-1);
    }

    //mui_list_view_sort(app->p_list_view, player_scene_file_browser_list_item_cmp);
}

void player_scene_file_browser_on_enter(void *user_data) {
    app_player_t *app = user_data;
    player_scene_file_browser_reload_folders(app);
    mui_list_view_set_selected_cb(app->p_list_view, player_scene_file_browser_on_selected);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, PLAYER_VIEW_ID_LIST);
}

void player_scene_file_browser_on_exit(void *user_data) { app_player_t *app = user_data; }