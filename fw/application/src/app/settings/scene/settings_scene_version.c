#include "app_settings.h"
#include "mini_app_launcher.h"
#include "settings_scene.h"
#include "utils.h"
#include "version2.h"
#include "i18n/language.h"

static void settings_scene_version_list_view_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                         mui_list_item_t *p_item) {
    app_settings_t *app = p_list_view->user_data;
    mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
}

void settings_scene_version_on_enter(void *user_data) {

    app_settings_t *app = user_data;
    char txt[32];
    sprintf(txt, "Version [%s]", version_get_version(NULL));
    mui_list_view_add_item(app->p_list_view, 0xe1c7, txt, (void *)0xFF);

    sprintf(txt, "BuildDate [%s]", version_get_builddate(NULL));
    mui_list_view_add_item(app->p_list_view, 0xe1c8, txt, (void *)0xFF);

    sprintf(txt, "GitHash [%s]", version_get_githash(NULL));
    mui_list_view_add_item(app->p_list_view, 0xe1c9, txt, (void *)0xFF);

    sprintf(txt, "Branch [%s]", version_get_gitbranch(NULL));
    mui_list_view_add_item(app->p_list_view, 0xe1ca, txt, (void *)0xFF);

    sprintf(txt, "Target [%d]", version_get_target(NULL));
    mui_list_view_add_item(app->p_list_view, 0xe1cb, txt, (void *)0xFF);

    sprintf(txt, "Dirty [%d]", version_get_dirty_flag(NULL));
    mui_list_view_add_item(app->p_list_view, 0xe1cc, txt, (void *)0xFF);

    mui_list_view_add_item(app->p_list_view, 0xe1cd, getLangString(_L_BACK), (void *)0);

    mui_list_view_set_selected_cb(app->p_list_view, settings_scene_version_list_view_on_selected);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SETTINGS_VIEW_ID_MAIN);
}

void settings_scene_version_on_exit(void *user_data) {
    app_settings_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
}