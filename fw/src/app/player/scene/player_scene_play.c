#include "app_player.h"
#include "player_scene.h"
#include "player_view.h"
#include "vfs.h"
#include "vfs_meta.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#define ICON_FOLDER 0xe1d6
#define ICON_FILE 0xe1ed
#define ICON_BACK 0xe069
#define ICON_ERROR 0xe1bb

static void player_scene_play_event_cb(player_view_event_t event, player_view_t *p_view) {
    app_player_t *app = p_view->user_data;
    player_view_stop(app->p_player_view);
    mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
}

void player_scene_play_on_enter(void *user_data) {
    app_player_t *app = user_data;
    player_view_set_event_cb(app->p_player_view, player_scene_play_event_cb);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, PLAYER_VIEW_ID_PLAYER);
    player_view_play(app->p_player_view, VFS_DRIVE_EXT, string_get_cstr(app->selected_file));
}

void player_scene_play_on_exit(void *user_data) { app_player_t *app = user_data; }