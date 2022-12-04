#include "app_settings.h"
#include "mini_app_registry.h"

#include "mui_include.h"

#include "mini_app_launcher.h"
#include "mui_list_view.h"
#include "mui_u8g2.h"

typedef enum { SETTINGS_VIEW_ID_MAIN } settings_view_id_t;

static void app_settings_on_run(mini_app_inst_t *p_app_inst);
static void app_settings_on_kill(mini_app_inst_t *p_app_inst);
static void app_settings_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event);

typedef struct {
    mui_list_view_t *p_list_view;
    mui_view_dispatcher_t *p_view_dispatcher;
} app_settings_t;

static void app_settings_list_view_on_selected(mui_list_view_t *p_list_view, mui_list_item_t *p_item) {
    app_settings_t *app = p_list_view->user_data;
    char txt[32];

    uint32_t selection = (uint32_t)p_item->user_data;
    switch (selection) {
    case 0:
        mui_u8g2_set_backlight(!mui_u8g2_get_backlight());
        sprintf(txt, "背光设置 [%s]", mui_u8g2_get_backlight() ? "开" : "关");
        string_set_str(p_item->text, txt);
        mui_update(mui());
        break;

    case 4:
        mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_SETTINGS);
        break;
    }
}

void app_settings_on_run(mini_app_inst_t *p_app_inst) {

    app_settings_t *p_app_handle = malloc(sizeof(app_settings_t));

    p_app_inst->p_handle = p_app_handle;
    p_app_handle->p_view_dispatcher = mui_view_dispatcher_create();
    p_app_handle->p_list_view = mui_list_view_create();

    char txt[32];
    sprintf(txt, "背光设置 [%s]", mui_u8g2_get_backlight() ? "开" : "关");

    mui_list_view_add_item(p_app_handle->p_list_view, 0xe1c8, txt, 0);
    mui_list_view_add_item(p_app_handle->p_list_view, 0xe069, "返回主菜单", 4);

    mui_list_view_set_selected_cb(p_app_handle->p_list_view, app_settings_list_view_on_selected);

    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, SETTINGS_VIEW_ID_MAIN,
                                 mui_list_view_get_view(p_app_handle->p_list_view));
    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher, MUI_LAYER_WINDOW);
    mui_view_dispatcher_switch_to_view(p_app_handle->p_view_dispatcher, SETTINGS_VIEW_ID_MAIN);
}

void app_settings_on_kill(mini_app_inst_t *p_app_inst) {
    app_settings_t *p_app_handle = p_app_inst->p_handle;

    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher, MUI_LAYER_WINDOW);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher);
    mui_list_view_free(p_app_handle->p_list_view);

    free(p_app_handle);

    p_app_inst->p_handle = NULL;
}

void app_settings_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event) {}

const mini_app_t app_settings_info = {.id = MINI_APP_ID_SETTINGS,
                                      .name = "系统设置",
                                      .icon = 0xe02b,
                                      .deamon = false,
                                      .sys = false,
                                      .run_cb = app_settings_on_run,
                                      .kill_cb = app_settings_on_kill,
                                      .on_event_cb = app_settings_on_event};
