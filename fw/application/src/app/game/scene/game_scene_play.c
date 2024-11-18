#include "app_game.h"
#include "game_scene.h"
#include "game_view.h"
#include "vfs.h"
#include "vfs_meta.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#define ICON_FOLDER 0xe1d6
#define ICON_FILE 0xe1ed
#define ICON_BACK 0xe069
#define ICON_ERROR 0xe1bb

static void game_scene_play_event_cb(game_view_event_t event, game_view_t *p_view) {
    app_game_t *app = p_view->user_data;
    game_view_stop(app->p_game_view);
    mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
}

void game_scene_play_on_enter(void *user_data) {
    app_game_t *app = user_data;
    game_view_set_event_cb(app->p_game_view, game_scene_play_event_cb);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, GAME_VIEW_ID_GAME);
    //game_view_play(app->p_game_view, VFS_DRIVE_EXT, string_get_cstr(app->selected_file));
}

void game_scene_play_on_exit(void *user_data) { app_game_t *app = user_data; }