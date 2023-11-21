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

static void chameleon_scene_menu_card_data_file_save_text_input_event_cb(mui_text_input_event_t event,
                                                                         mui_text_input_t *p_text_input) {
    app_chameleon_t *app = p_text_input->user_data;
    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        const char *text = mui_text_input_get_input_text(p_text_input);
        if (strlen(text) > 0) {
            char path[VFS_MAX_PATH_LEN];
            vfs_driver_t *p_driver = vfs_get_default_driver();
            uint8_t *tag_buffer = tag_helper_get_active_tag_memory_data();
            size_t tag_data_size = tag_helper_get_active_tag_data_size();
            sprintf(path, "%s/%s", CHELEMEON_DUMP_FOLDER, text);
            int32_t err = p_driver->write_file_data(path, tag_buffer, tag_data_size);
            if (err < 0) {
                mui_toast_view_show(app->p_toast_view, _T(APP_CHAMELEON_CARD_DATA_SAVE_FAILED));
                return;
            }
            mui_toast_view_show(app->p_toast_view, _T(APP_CHAMELEON_CARD_DATA_SAVE_SUCCESS));
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        } else {
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }
    }
}

void chameleon_scene_menu_card_data_file_save_on_enter(void *user_data) {
    app_chameleon_t *app = user_data;
    char file_name[32];

    sprintf(file_name, "%02d.bin", tag_emulation_get_slot());

    mui_text_input_set_header(app->p_text_input, _T(APP_CHAMELEON_CARD_DATA_SAVE_INPUT_FILE_NAME));
    mui_text_input_set_input_text(app->p_text_input, file_name);
    mui_text_input_set_event_cb(app->p_text_input, chameleon_scene_menu_card_data_file_save_text_input_event_cb);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, CHAMELEON_VIEW_ID_TEXT_INPUT);
}

void chameleon_scene_menu_card_data_file_save_on_exit(void *user_data) {
    app_chameleon_t *app = user_data;
}