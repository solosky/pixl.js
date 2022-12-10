#include "amiibo_data.h"
#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "cwalk.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "spiffs.h"
#include <string.h>

static void amiibo_scene_amiibo_list_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                 mui_list_item_t *p_item) {
    app_amiibo_t *app = p_list_view->user_data;
    uint32_t idx = (uint32_t)p_item->user_data;

    char path[64] = {0};
    if (idx != 0xFFFF) {
        size_t path_pos;
        char *file = string_get_cstr(app->current_file);
        cwk_path_get_dirname(file, &path_pos);
        strncpy(path, file, path_pos);
        strcat(path, string_get_cstr(p_item->text));

        string_set_str(app->current_file, path);
    }

    if (event == MUI_LIST_VIEW_EVENT_SELECTED) {
        if (idx == 0xFFFF) {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FOLDER_LIST);
        } else {
            spiffs *fs = spiffs_man_get_fs(app->current_drive);
            spiffs_file fd = SPIFFS_open(fs, path, SPIFFS_RDONLY, 0);
            if (fd) {
                int res = SPIFFS_read(fs, fd, &app->ntag, sizeof(ntag_t));
                NRF_LOG_INFO("ntag read: %d", res);
                SPIFFS_close(fs, fd);
            }
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_AMIIBO_DETAIL);
        }
    } else {
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_AMIIBO_LIST_MENU);
    }
}

void amiibo_scene_amiibo_list_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;
    ntag_t ntag;
    mui_list_view_add_item(app->p_list_view, 0xe1d6, "..", (void *)0xFFFF);

    size_t path_pos;
    char path[64];

    char *file = string_get_cstr(app->current_file);
    cwk_path_get_dirname(file, &path_pos);
    strncpy(path, file, path_pos);

    spiffs *fs = spiffs_man_get_fs(app->current_drive);
    spiffs_DIR d;
    struct spiffs_dirent e;
    struct spiffs_dirent *pe = &e;
    SPIFFS_opendir(fs, path, &d);
    while ((pe = SPIFFS_readdir(&d, pe))) {
        const char *basename;
        size_t length;
        cwk_path_get_basename(pe->name, &basename, &length);
        memset(path, 0, sizeof(path));
        strncpy(path, basename, length);
        NRF_LOG_INFO("%s [%04x] size:%i\n", nrf_log_push(pe->name), pe->obj_id, pe->size);
        if (strcmp(path, ".folder") != 0) {
            mui_list_view_add_item(app->p_list_view, 0xe1d6, path, pe->obj_id);
        }
    }
    SPIFFS_closedir(&d);

    // for (uint32_t i = 0; i < 10; i++) {
    //     ntag_store_read_default(i, &ntag);
    //     uint32_t head = to_little_endian_int32(&ntag.data[84]);
    //     uint32_t tail = to_little_endian_int32(&ntag.data[88]);
    //     const amiibo_data_t *amd = find_amiibo_data(head, tail);
    //     char text[32];
    //     if (amd) {
    //         sprintf(text, "%02d %s", i + 1, amd->name);
    //     } else {
    //         sprintf(text, "%02d %s", i + 1, "空标签");
    //     }
    //     mui_list_view_add_item(app->p_list_view, 0xe1ed, text, (void *)i);
    // }
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