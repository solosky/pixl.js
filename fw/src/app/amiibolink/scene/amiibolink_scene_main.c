#include "amiibolink_scene.h"
#include "app_amiibolink.h"
#include "amiibolink_view.h"
#include "vfs.h"
#include "vfs_meta.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#define ICON_FOLDER 0xe1d6
#define ICON_FILE 0xe1ed
#define ICON_BACK 0xe069
#define ICON_ERROR 0xe1bb

static void amiibolink_scene_main_event_cb(amiibolink_view_event_t event, amiibolink_view_t *p_view) {
    app_amiibolink_t *app = p_view->user_data;
    mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBOLINK_SCENE_MENU);
}

void amiibolink_scene_main_on_enter(void *user_data) {
    app_amiibolink_t *app = user_data;
    amiibolink_view_set_event_cb(app->p_amiibolink_view, amiibolink_scene_main_event_cb);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBOLINK_VIEW_ID_MAIN);
}

void amiibolink_scene_main_on_exit(void *user_data) { app_amiibolink_t *app = user_data; }