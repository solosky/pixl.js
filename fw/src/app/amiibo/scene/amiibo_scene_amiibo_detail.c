#include "amiibo_data.h"
#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "cwalk2.h"
#include "mui_list_view.h"
#include "vfs.h"

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
            }
        }

        // todo ..
    }

    mui_update(mui());
}

// static void amiibo_detail_view_update(mui_view_t *p_view, input_key_t key) {
//     amiibo_detail_view_t *p_amiibo_detail_view = p_view->user_data;
//     app_amiibo_t *app = p_amiibo_detail_view->user_data;
//     bool updated = false;
//     if (key == INPUT_KEY_LEFT && app->current_file_index > 0) {
//         app->current_file_index--;
//         updated = true;
//     } else if (key == INPUT_KEY_RIGHT && app->current_file_index < app->current_files_size - 1 ) {
//         app->current_file_index++;
//         updated = true;
//     }

//     // reload
//     if (updated) {
//         vos_driver_t *p_driver = vos_get_driver(app->current_drive);
//         m_string_set_cstr(app->current_file, app->current_files[app->current_file_index].name);
//         int32_t res = p_driver->read_object(VOS_BUCKET_AMIIBO, string_get_cstr(app->current_folder),
//                                             string_get_cstr(app->current_file), &app->ntag, sizeof(ntag_t));
//         p_amiibo_detail_view->amiibo->index = app->current_file_index;
//         if (res > 0) {
//             mui_update(mui());
//         }
//     }
// }

static void app_amiibo_detail_view_on_event(amiibo_detail_view_event_t event, amiibo_detail_view_t *p_view) {
    app_amiibo_t *app = p_view->user_data;
    if (event == AMIIBO_DETAIL_VIEW_EVENT_MENU) {
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_AMIIBO_DETAIL_MENU);
    }else if(event == AMIIBO_DETAIL_VIEW_EVENT_PREV){
        //TODO ..
    }else if(event == AMIIBO_DETAIL_VIEW_EVENT_NEXT){
        //TODO ..
    }
}

void amiibo_scene_amiibo_detail_on_enter(void *user_data) {
    app_amiibo_t *app = user_data;
    app->p_amiibo_detail_view->ntag = &app->ntag;
    app->p_amiibo_detail_view->event_cb = app_amiibo_detail_view_on_event;

    ntag_emu_set_tag(app->p_amiibo_detail_view->ntag);
    ntag_emu_set_update_cb(ntag_update_cb, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_DETAIL);
}

void amiibo_scene_amiibo_detail_on_exit(void *user_data) { ntag_emu_set_update_cb(NULL, NULL); }