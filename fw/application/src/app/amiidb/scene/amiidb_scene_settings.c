#include "amiibo_helper.h"
#include "amiidb_scene.h"
#include "app_amiidb.h"
#include "cwalk2.h"
#include "settings.h"
#include "mui_icons.h"

#include "i18n/language.h"


static void amiidb_scene_settings_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                              mui_list_item_t *p_item) {
    app_amiidb_t *app = p_list_view->user_data;

    switch (p_item->icon) {
    case ICON_EXIT:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;

    case ICON_SLOT: {
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIDB_SCENE_SETTINGS_SLOT_NUM);
    } break;
    }
}

void amiidb_scene_settings_on_enter(void *user_data) {
    app_amiidb_t *app = user_data;

    char txt[32];
    settings_data_t *p_settings = settings_get_data();
    mui_list_view_clear_items(app->p_list_view);

    sprintf(txt, "Keys [%s]", amiibo_helper_is_key_loaded() ? "已加载" : "未加载");
    mui_list_view_add_item(app->p_list_view, ICON_KEY, txt, (void *)0);
    sprintf(txt, "数据槽位 [%d]", p_settings->amiidb_data_slot_num);
    mui_list_view_add_item(app->p_list_view, ICON_SLOT, txt, (void *)0);
    mui_list_view_add_item(app->p_list_view, ICON_EXIT, "[返回]", (void *)0);

    mui_list_view_set_selected_cb(app->p_list_view, amiidb_scene_settings_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIDB_VIEW_ID_LIST);
}

void amiidb_scene_settings_on_exit(void *user_data) {
    app_amiidb_t *app = user_data;
    settings_save();
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}
