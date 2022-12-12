#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "mui_text_input.h"
#include "nrf_log.h"
#include "vos.h"

static void amiibo_scene_folder_list_text_input_event_cb(mui_text_input_event_t event, mui_text_input_t *p_text_input) {
    app_amiibo_t *app = p_text_input->user_data;
    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        const char *input_text = mui_text_input_get_input_text(p_text_input);
        if (strlen(input_text) > 0) {
            vos_driver_t *p_driver = vos_get_driver(app->current_drive);
            int32_t res = p_driver->create_folder(VOS_BUCKET_AMIIBO, input_text);
            if (res == VOS_OK) {
                mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FOLDER_LIST);
            }
        }
    }
}

void amiibo_scene_folder_list_name_input_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_text_input_set_header(app->p_text_input, "Input Folder Name:");
    mui_text_input_set_input_text(app->p_text_input, "");
    mui_text_input_set_event_cb(app->p_text_input, amiibo_scene_folder_list_text_input_event_cb);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_INPUT);
}

void amiibo_scene_folder_list_name_input_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_text_input_reset(app->p_text_input);
    mui_text_input_set_event_cb(app->p_text_input, NULL);
}