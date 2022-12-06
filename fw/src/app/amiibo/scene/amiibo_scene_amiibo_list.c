#include "amiibo_data.h"
#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "mui_list_view.h"

static void amiibo_scene_amiibo_list_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                 mui_list_item_t *p_item) {
    app_amiibo_t *app = p_list_view->user_data;
    uint32_t idx = (uint32_t)p_item->user_data;
    if (idx == 0xFFFF) {
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FOLDER_LIST);
    } else {
        ntag_store_read_default((uint8_t)p_item->user_data, &app->ntag);
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_AMIIBO_DETAIL);
    }
}

void amiibo_scene_amiibo_list_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;
    ntag_t ntag;
    mui_list_view_add_item(app->p_list_view, 0xe1d6, "..", (void *)0xFFFF);
    for (uint32_t i = 0; i < 10; i++) {
        ntag_store_read_default(i, &ntag);
        uint32_t head = to_little_endian_int32(&ntag.data[84]);
        uint32_t tail = to_little_endian_int32(&ntag.data[88]);
        const amiibo_data_t *amd = find_amiibo_data(head, tail);
        char text[32];
        if (amd) {
            sprintf(text, "%02d %s", i + 1, amd->name);
        } else {
            sprintf(text, "%02d %s", i + 1, "空标签");
        }
        mui_list_view_add_item(app->p_list_view, 0xe1ed, text, (void *)i);
    }
    mui_list_view_set_selected_cb(app->p_list_view, amiibo_scene_amiibo_list_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
}

void amiibo_scene_amiibo_list_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}