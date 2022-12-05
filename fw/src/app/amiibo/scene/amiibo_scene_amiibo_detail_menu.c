#include "amiibo_data.h"
#include "amiibo_helper.h"
#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "app_error.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "ntag_store.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

static void amiibo_scene_amiibo_detail_menu_on_selected(mui_list_view_t *p_list_view, mui_list_item_t *p_item) {
    app_amiibo_t *app = p_list_view->user_data;

    uint32_t selection = (uint32_t)p_item->user_data;
    switch (selection) {
    case 0:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_AMIIBO_LIST);
        break;
    case 1: {
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
            // ntag_emu_set_uuid_only(&ntag_new);
            // ntag_emu_set_tag(&ntag_new);
            memcpy(&app->ntag, &ntag_new, sizeof(ntag_t));
            NRF_LOG_INFO("reset uuid success");
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }
    } break;

    case 2:
        ntag_store_reset(app->ntag.index, &(app->ntag));
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;

    case 3:
        break;

    case 4:
        mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_AMIIBO);
        break;
    }
}

void amiibo_scene_amiibo_detail_menu_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;

    mui_list_view_add_item(app->p_list_view, 0xe069, "返回列表", 0);
    mui_list_view_add_item(app->p_list_view, 0xe1c5, "随机生成", 1);
    mui_list_view_add_item(app->p_list_view, 0xe1c6, "重置标签", 2);
    mui_list_view_add_item(app->p_list_view, 0xe1c7, "删除标签", 3);
    mui_list_view_add_item(app->p_list_view, 0xe1c8, "返回主菜单", 4);

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