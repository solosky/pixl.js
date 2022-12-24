#include "amiibo_data.h"
#include "amiibo_helper.h"
#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "app_error.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "vos.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

enum amiibo_detail_menu_t {
    AMIIBO_DETAIL_MENU_RAND_UID,
    AMIIBO_DETAIL_MENU_REMOVE_UID,
    AMIIBO_DETAIL_MENU_BACK_AMIIBO_LIST,
    AMIIBO_DETAIL_MENU_BACK_MAIN_MENU,
};

static void amiibo_scene_amiibo_detail_menu_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                        mui_list_item_t *p_item) {
    app_amiibo_t *app = p_list_view->user_data;

    uint32_t selection = (uint32_t)p_item->user_data;
    vos_driver_t *p_driver = vos_get_driver(app->current_drive);
    const char *file = string_get_cstr(app->current_file);
    const char *folder = string_get_cstr(app->current_folder);

    switch (selection) {
    case AMIIBO_DETAIL_MENU_BACK_AMIIBO_LIST:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_AMIIBO_LIST);
        break;
    case AMIIBO_DETAIL_MENU_RAND_UID: {
        ret_code_t err_code;
        ntag_t ntag_new;
        ntag_t *ntag_current = &app->ntag;
        uint32_t head = to_little_endian_int32(&ntag_current->data[84]);
        uint32_t tail = to_little_endian_int32(&ntag_current->data[88]);

        memcpy(&ntag_new, ntag_current, sizeof(ntag_t));

        const amiibo_data_t *amd = find_amiibo_data(head, tail);
        if (amd == NULL) {
            return;
        }

        err_code = ntag_store_uuid_rand(&ntag_new);
        APP_ERROR_CHECK(err_code);

        // sign new
        err_code = amiibo_helper_sign_new_ntag(ntag_current, &ntag_new);
        if (err_code == NRF_SUCCESS) {
            memcpy(&app->ntag, &ntag_new, sizeof(ntag_t));
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }
    } break;

    case AMIIBO_DETAIL_MENU_REMOVE_UID: {
        
    } break;

    case AMIIBO_DETAIL_MENU_BACK_MAIN_MENU:
        mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_AMIIBO);
        break;
    }
}

void amiibo_scene_amiibo_detail_menu_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;

    mui_list_view_add_item(app->p_list_view, 0xe1c5, "随机生成", (void *)AMIIBO_DETAIL_MENU_RAND_UID);
    mui_list_view_add_item(app->p_list_view, 0xe1c6, "删除标签", (void *)AMIIBO_DETAIL_MENU_REMOVE_UID);
    mui_list_view_add_item(app->p_list_view, 0xe069, "返回列表", (void *)AMIIBO_DETAIL_MENU_BACK_AMIIBO_LIST);
    mui_list_view_add_item(app->p_list_view, 0xe1c8, "返回主菜单", (void *)AMIIBO_DETAIL_MENU_BACK_MAIN_MENU);

    mui_list_view_set_selected_cb(app->p_list_view, amiibo_scene_amiibo_detail_menu_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
}

void amiibo_scene_amiibo_detail_menu_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}