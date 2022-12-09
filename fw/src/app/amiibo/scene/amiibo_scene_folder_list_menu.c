#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "mui_list_view.h"
#include "spiffs_manager.h"

static void amiibo_scene_folder_list_menu_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                      mui_list_item_t *p_item) {
    app_amiibo_t *app = p_list_view->user_data;
    uint32_t index = (uint32_t)p_item->user_data;
    switch (index) {
    case 0:{
        spiffs *fs = spiffs_man_get_fs(app->current_drive);
        spiffs_file fd = SPIFFS_open(fs, "/amiibo/new/.folder", SPIFFS_CREAT | SPIFFS_RDWR, 0);
        APP_ERROR_CHECK(fd);
        int res = SPIFFS_write(fs, fd, "This is a folder place holder", 128);
        APP_ERROR_CHECK(res);
        res = SPIFFS_close(fs, fd);
        APP_ERROR_CHECK(res);
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }

    case 99:
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    }
}

void amiibo_scene_folder_list_menu_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;

    mui_list_view_add_item(app->p_list_view, 0xe1c8, "新建文件夹", (void *)0);
    mui_list_view_add_item(app->p_list_view, 0xe1c9, "删除文件夹", (void *)1);
    mui_list_view_add_item(app->p_list_view, 0xe1ca, "清空文件夹", (void *)2);
    mui_list_view_add_item(app->p_list_view, 0xe069, "返回列表", (void *)99);

    mui_list_view_set_selected_cb(app->p_list_view, amiibo_scene_folder_list_menu_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_LIST);
}

void amiibo_scene_folder_list_menu_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}