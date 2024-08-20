#include "amiibo_helper.h"
#include "amiibo_scene.h"
#include "app_amiibo.h"
#include "app_timer.h"
#include "cwalk2.h"
#include "db_header.h"
#include "i18n/language.h"
#include "mui_list_view.h"
#include "nrf_log.h"
#include "ntag_emu.h"
#include "ntag_store.h"
#include "settings.h"
#include "vfs.h"
#include "vfs_meta.h"

#define NRF_ERR_NOT_AMIIBO -1000
#define NRF_ERR_READ_ERROR -1001

APP_TIMER_DEF(m_amiibo_gen_delay_timer);

static void amiibo_scene_amiibo_detail_reload_files(app_amiibo_t *app);

static void amiibo_scene_amiibo_detail_msg_box_error_cb(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box) {
    app_amiibo_t *app = p_msg_box->user_data;
    mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBO_SCENE_FILE_BROWSER);
}

static void amiibo_scene_amiibo_detail_reload_error(app_amiibo_t *app, const char *path, int32_t err_code) {

    char msg[64];
    strcpy(msg, path);
    strcat(msg, "\n");
    if (err_code == NRF_ERR_NOT_AMIIBO) {
        strcat(msg, getLangString(_L_NOT_AMIIBO_FILE));
    } else if (err_code == NRF_ERR_READ_ERROR) {
        strcat(msg, getLangString(_L_READ_FILE_FAILED));
    } else {
        strcat(msg, getLangString(_L_READ_FILE_FAILED));
    }

    mui_msg_box_set_header(app->p_msg_box, getLangString(_L_ERR));
    mui_msg_box_set_message(app->p_msg_box, msg);
    mui_msg_box_set_btn_text(app->p_msg_box, NULL, getLangString(_L_BACK), NULL);
    mui_msg_box_set_btn_focus(app->p_msg_box, 1);
    mui_msg_box_set_event_cb(app->p_msg_box, amiibo_scene_amiibo_detail_msg_box_error_cb);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_MSG_BOX);
}

static int32_t ntag_read(vfs_driver_t *p_vfs_driver, const char *path, ntag_t *ntag) {

    vfs_obj_t obj;
    int32_t res;

    memset(ntag, 0, sizeof(ntag_t));
    res = p_vfs_driver->stat_file(path, &obj);
    if (res != VFS_OK) {
        return NRF_ERR_READ_ERROR;
    }

    if (obj.size != 540 && obj.size != 532 && obj.size != 572) {
        return NRF_ERR_NOT_AMIIBO;
    }

    uint8_t meta_size = obj.meta[0];
    if (meta_size > 0 && meta_size < 0xFF) {
        memcpy(ntag->notes, obj.meta + 3, meta_size - 2);
    }

    vfs_meta_t meta;
    memset(&meta, 0, sizeof(vfs_meta_t));
    vfs_meta_decode(obj.meta, sizeof(obj.meta), &meta);
    if (meta.has_notes) {
        memcpy(ntag->notes, meta.notes, strlen(meta.notes));
    }

    if (meta.has_flags && (meta.flags & VFS_OBJ_FLAG_READONLY)) {
        ntag->read_only = true;
    }

    res = p_vfs_driver->read_file_data(path, ntag->data, 540);
    if (res != 540 && res != 532) {
        return NRF_ERR_READ_ERROR;
    }
    return NRF_SUCCESS;
}

static void ntag_gen(void *p_context) {
    ret_code_t err_code;
    app_amiibo_t *app = p_context;
    ntag_t *ntag_current = &app->ntag;

    err_code = amiibo_helper_rand_amiibo_uuid(ntag_current);
    if (err_code == NRF_SUCCESS) {
        ntag_emu_set_tag(&app->ntag);
        mui_update(mui());
    }
}

static void ntag_update(app_amiibo_t *app, ntag_t *p_ntag) {
    amiibo_detail_view_t *p_amiibo_detail_view = app->p_amiibo_detail_view;
    memcpy(p_amiibo_detail_view->ntag, p_ntag, sizeof(ntag_t));

    vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);

    char path[VFS_MAX_PATH_LEN];
    cwalk_append_segment(path, string_get_cstr(app->current_folder), string_get_cstr(app->current_file));

    // save to fs
    int32_t res = p_driver->write_file_data(path, p_ntag->data, sizeof(p_ntag->data));
    if (res > 0) {
        uint32_t head = to_little_endian_int32(&p_ntag->data[84]);
        uint32_t tail = to_little_endian_int32(&p_ntag->data[88]);

        const db_amiibo_t *amd = get_amiibo_by_id(head, tail);

        if (amd && strncmp(string_get_cstr(app->current_file), "new", 3) == 0) {
            char new_path[VFS_MAX_PATH_LEN];
            char new_name[VFS_MAX_NAME_LEN];
            snprintf(new_name, sizeof(new_name), "%s.bin", amd->name_en);
            cwalk_append_segment(new_path, string_get_cstr(app->current_folder), new_name);
            res = p_driver->rename_file(path, new_path);

            if (res == VFS_OK) {
                // todo ..
                string_set_str(app->current_file, new_name);
                amiibo_detail_view_set_file_name(p_amiibo_detail_view, new_name);
                amiibo_scene_amiibo_detail_reload_files(app);
            }
        }
    }

    mui_update(mui());
}

static void ntag_update_cb(ntag_event_type_t type, void *context, ntag_t *p_ntag) {

    app_amiibo_t *app = context;

    if (type == NTAG_EVENT_TYPE_WRITTEN) {
        ntag_update(app, p_ntag);
    } else if (type == NTAG_EVENT_TYPE_READ) {
        settings_data_t *p_settings = settings_get_data();
        if (p_settings->auto_gen_amiibo) {
            app_timer_stop(m_amiibo_gen_delay_timer);
            app_timer_start(m_amiibo_gen_delay_timer, APP_TIMER_TICKS(1000), app);
        }
    }
}

static bool amiibo_scene_amiibo_detail_reload_ntag(app_amiibo_t *app, const char *file_name) {
    char path[VFS_MAX_PATH_LEN];

    if (strlen(file_name) == 0) {
        return false;
    }

    cwalk_append_segment(path, string_get_cstr(app->current_folder), file_name);

    vfs_driver_t *p_vfs_driver = vfs_get_driver(app->current_drive);
    int32_t err = ntag_read(p_vfs_driver, path, &app->ntag);
    if (err != NRF_SUCCESS) {
        amiibo_scene_amiibo_detail_reload_error(app, file_name, err);
        string_set_str(app->current_file, "");
        return false;
    }
    string_set_str(app->current_file, file_name);
    amiibo_detail_view_set_file_name(app->p_amiibo_detail_view, file_name);
    amiibo_detail_view_set_ntag(app->p_amiibo_detail_view, &app->ntag);
    ntag_emu_set_tag(app->p_amiibo_detail_view->ntag);

    return true;
}

static int amiibo_scene_amiibo_detail_list_item_cmp(const string_t *a, const string_t *b) { return string_cmp(*a, *b); }

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
        while ((res = p_vfs_driver->read_dir(&dir, &obj)) == VFS_OK) {
            vfs_meta_t meta;
            memset(&meta, 0, sizeof(vfs_meta_t));
            vfs_meta_decode(obj.meta, sizeof(obj.meta), &meta);
            if (obj.type == VFS_TYPE_REG &&
                (obj.size == NTAG_DATA_SIZE || obj.size == NTAG_TAGMO_DATA_SIZE ||
                 obj.size == NTAG_THENAYA_DATA_SIZE) &&
                (!meta.has_flags || !(meta.flags & VFS_OBJ_FLAG_HIDDEN))) {
                string_set_str(file_name, obj.name);
                string_array_push_back(app->amiibo_files, file_name);
            }
        }
        p_vfs_driver->close_dir(&dir);
    }

    string_array_special_sort(app->amiibo_files, amiibo_scene_amiibo_detail_list_item_cmp);

    // load amiibo detail
    string_array_it_t it;
    string_array_it(it, app->amiibo_files);
    while (!string_array_end_p(it)) {
        string_t *item = string_array_ref(it);
        if (string_cmp(*item, app->current_file) == 0) {
            amiibo_detail_view_set_focus(app->p_amiibo_detail_view, focus);
            break;
        }
        focus++;
        string_array_next(it);
    }

    if (focus >= string_array_size(app->amiibo_files)) {
        amiibo_detail_view_set_focus(app->p_amiibo_detail_view, 0);
    }

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
        if (!amiibo_scene_amiibo_detail_reload_ntag(app, string_get_cstr(app->current_file))) {
            return;
        }
        amiibo_scene_amiibo_detail_reload_files(app);
        app->reload_amiibo_files = false;
    }

    ntag_emu_set_update_cb(ntag_update_cb, app);

    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBO_VIEW_ID_DETAIL);

    int32_t err_code = app_timer_create(&m_amiibo_gen_delay_timer, APP_TIMER_MODE_SINGLE_SHOT, ntag_gen);
    APP_ERROR_CHECK(err_code);
}

void amiibo_scene_amiibo_detail_on_exit(void *user_data) {
    app_amiibo_t *app = user_data;
    ntag_emu_set_update_cb(NULL, NULL);
    app_timer_stop(m_amiibo_gen_delay_timer);
}
