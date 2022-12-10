#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "ntag_def.h"
#include "spiffs_manager.h"

static void amiibo_scene_amiibo_list_menu_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                      mui_list_item_t *p_item) {
    app_amiibo_t *app = p_list_view->user_data;
    uint32_t index = (uint32_t)p_item->user_data;
    size_t path_pos;
    char path[64];
    memset(path, 0, sizeof(path));

    switch (index) {
    case 0: {

        char *file = string_get_cstr(app->current_file);
        cwk_path_get_dirname(file, &path_pos);
        strncpy(path, file, path_pos);
        strcat(path, "001.bin");

        spiffs *fs = spiffs_man_get_fs(app->current_drive);
        spiffs_file fd = SPIFFS_open(fs, path, SPIFFS_CREAT | SPIFFS_RDWR, 0);

        extern const ntag_t default_ntag215;

        ntag_t tmp_ntag;
        memcpy(&tmp_ntag, &default_ntag215, sizeof(ntag_t));
        int res = SPIFFS_write(fs, fd, &tmp_ntag, sizeof(tmp_ntag));
        if (res < 0) {
            NRF_LOG_INFO("spiffs write error:%d", res);
        }
        res = SPIFFS_close(fs, fd);
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }

    case 1: {

        char *file = string_get_cstr(app->current_file);

        spiffs *fs = spiffs_man_get_fs(app->current_drive);
        int res = SPIFFS_remove(fs, file);
        NRF_LOG_INFO("spiffs remove:%s %d", nrf_log_push(file), res);
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
    } break;

    case 99:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

void amiibo_scene_amiibo_list_menu_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;

    mui_list_view_add_item(app->p_list_view, 0xe1c8, "新建amiibo", (void *)0);
    mui_list_view_add_item(app->p_list_view, 0xe1c9, "删除amiibo", (void *)1);
    mui_list_view_add_item(app->p_list_view, 0xe069, "返回列表", (void *)99);

    mui_list_view_set_selected_cb(app->p_list_view, amiibo_scene_amiibo_list_menu_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
}

void amiibo_scene_amiibo_list_menu_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}