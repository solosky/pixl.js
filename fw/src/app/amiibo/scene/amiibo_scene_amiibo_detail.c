#include "amiibo_data.h"
#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "cwalk2.h"
#include "mui_list_view.h"
#include "vfs.h"

static void amiibo_scene_amiibo_detail_reload_files(app_amiibo_t *app);

static void ntag_update_cb(void *context, ntag_t *p_ntag) {
    app_amiibo_t *app = context;
    amiibo_detail_view_t *p_amiibo_detail_view = app->p_amiibo_detail_view;
    memcpy(p_amiibo_detail_view->ntag, p_ntag, sizeof(ntag_t));

    vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);

    char path[VFS_MAX_PATH_LEN];
    cwalk_append_segment(path, string_get_cstr(app->current_folder), string_get_cstr(app->current_file));

    // save to fs
    int32_t res = p_driver->write_file_data(path, p_ntag, sizeof(ntag_t));
    if (res > 0) {
        uint32_t head = to_little_endian_int32(&p_ntag->data[84]);
        uint32_t tail = to_little_endian_int32(&p_ntag->data[88]);

        const amiibo_data_t *amd = find_amiibo_data(head, tail);

        if (amd) {
            char new_path[VFS_MAX_PATH_LEN];
            char new_name[VFS_MAX_NAME_LEN];
            snprintf(new_name, sizeof(new_name), "%s.bin", amd->name);
            cwalk_append_segment(new_path, string_get_cstr(app->current_folder), new_name);
            res = p_driver->rename_file(path, new_path);

            if (res == VFS_OK) {
                // todo ..
                string_set_str(app->current_file, new_name);
                amiibo_scene_amiibo_detail_reload_files(app);
            }
        }
    }

    mui_update(mui());
}

static void amiibo_scene_amiibo_detail_reload_ntag(app_amiibo_t *app, const char *file_name) {
    char path[VFS_MAX_PATH_LEN];

    if (strlen(file_name) == 0) {
        return;
    }

    snprintf(path, sizeof(path), "%s/%s", string_get_cstr(app->current_folder), file_name);

    vfs_driver_t *p_vfs_driver = vfs_get_driver(app->current_drive);
    int32_t res = p_vfs_driver->read_file_data(path, &app->ntag, sizeof(ntag_t));
    app->ntag.index = 1;
    if (res <= 0) {
        // TODO error handling..
        return;
    }
    string_set_str(app->current_file, file_name);
    amiibo_detail_view_set_ntag(app->p_amiibo_detail_view, &app->ntag);
    ntag_emu_set_tag(app->p_amiibo_detail_view->ntag);
}

static void amiibo_scene_amiibo_detail_reload_files(app_amiibo_t *app) {
    vfs_dir_t dir;
    vfs_obj_t obj;
    string_t file_name;
    uint32_t focus = 0;

    // query amiibo list
    string_init(file_name);
    string_array_reset(app->amiibo_files);
    vfs_driver_t *p_vfs_driver = vfs_get_driver(app->current_drive);

    int32_t res = p_vfs_driver->open_dir(string_get_cstr(app->current_folder), &dir);
    if (res == VFS_OK) {
        while (res = p_vfs_driver->read_dir(&dir, &obj) == VFS_OK) {
            if (obj.type == VFS_TYPE_REG && obj.size == NTAG_DATA_SIZE) {
                string_set_str(file_name, obj.name);
                string_array_push_back(app->amiibo_files, file_name);
                if (string_cmp(file_name, app->current_file) == 0) {
                    focus = string_array_size(app->amiibo_files) - 1;
                }
            }
        }
        p_vfs_driver->close_dir(&dir);
    }
    amiibo_detail_view_set_focus(app->p_amiibo_detail_view, focus);
    amiibo_detail_view_set_max_ntags(app->p_amiibo_detail_view, string_array_size(app->amiibo_files));
}

static void app_amiibo_detail_view_on_event(amiibo_detail_view_event_t event, amiibo_detail_view_t *p_view) {
    app_amiibo_t *app = p_view->user_data;
    if (event == AMIIBO_DETAIL_VIEW_EVENT_MENU) {
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_AMIIBO_DETAIL_MENU);
    } else if (event == AMIIBO_DETAIL_VIEW_EVENT_UPDATE) {
        uint8_t focus = amiibo_detail_view_get_focus(app->p_amiibo_detail_view);
        amiibo_scene_amiibo_detail_reload_ntag(app, string_get_cstr(*string_array_get(app->amiibo_files, focus)));
    }
}

void amiibo_scene_amiibo_detail_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;
    app->p_amiibo_detail_view->ntag = &app->ntag;
    app->p_amiibo_detail_view->event_cb = app_amiibo_detail_view_on_event;
    amiibo_detail_view_set_event_cb(app->p_amiibo_detail_view, app_amiibo_detail_view_on_event);

    if (app->reload_amiibo_files) {
        amiibo_scene_amiibo_detail_reload_ntag(app, string_get_cstr(app->current_file));
        amiibo_scene_amiibo_detail_reload_files(app);
        app->reload_amiibo_files = false;
    }
    ntag_emu_set_update_cb(ntag_update_cb, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_DETAIL);
}

void amiibo_scene_amiibo_detail_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    ntag_emu_set_update_cb(NULL, NULL);
    // string_array_clear(app->amiibo_files);
}