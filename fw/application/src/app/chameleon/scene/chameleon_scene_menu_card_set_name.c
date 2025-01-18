#include "app_chameleon.h"
#include "chameleon_scene.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "amiibo_helper.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "i18n/language.h"
#include "settings.h"

#include "mui_icons.h"
#include "tag_helper.h"

#include "fds_utils.h"

typedef enum {
    CHAMELEON_MENU_BACK,
    CHAMELEON_MENU_FILE,
} chameleon_menu_item_t;

static void chameleon_scene_menu_card_set_name_text_input_event_cb(mui_text_input_event_t event,
                                                                   mui_text_input_t *p_text_input) {
    app_chameleon_t *app = p_text_input->user_data;
    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        const char *text = mui_text_input_get_input_text(p_text_input);
        if (strlen(text) > 0) {
            int32_t err = tag_helper_set_nickname(text);
            if (err != 0) {
                mui_toast_view_show(app->p_toast_view, _T(APP_CHAMELEON_CARD_SET_NICK_FAILED));
                mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
                return;
            }
            mui_toast_view_show(app->p_toast_view, _T(APP_CHAMELEON_CARD_SET_NICK_SUCCESS));
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        } else {
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }
    }
}

void chameleon_scene_menu_card_set_name_on_enter(void *user_data) {
    app_chameleon_t *app = user_data;
    char card_name[32];

    tag_helper_get_nickname(card_name, sizeof(card_name));

    mui_text_input_set_header(app->p_text_input, _T(APP_CHAMELEON_CARD_INPUT_NICK));
    mui_text_input_set_input_text(app->p_text_input, card_name);
    mui_text_input_set_event_cb(app->p_text_input, chameleon_scene_menu_card_set_name_text_input_event_cb);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, CHAMELEON_VIEW_ID_TEXT_INPUT);
}

void chameleon_scene_menu_card_set_name_on_exit(void *user_data) {
    app_chameleon_t *app = user_data;
    mui_list_view_clear_items(app->p_list_view);
}