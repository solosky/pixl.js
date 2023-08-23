#include "amiibolink_scene.h"
#include "amiibolink_view.h"
#include "app_amiibolink.h"
#include "ble_amiibolink.h"
#include "ble_main.h"
#include "vfs.h"

#include "amiibo_helper.h"
#include "mini_app_launcher.h"
#include "mini_app_registry.h"
#include "ntag_emu.h"
#include "ntag_store.h"

#include "nrf_log.h"

#include "settings.h"

#define ICON_FOLDER 0xe1d6
#define ICON_FILE 0xe1ed
#define ICON_BACK 0xe069
#define ICON_ERROR 0xe1bb

#define DEFAULT_NTAG_INDEX 99
#define MAX_NTAG_INDEX 26

APP_TIMER_DEF(m_amiibo_gen_delay_timer);

static void ntag_generate_cb() {
    ret_code_t err_code = amiibo_helper_rand_amiibo_uuid(ntag_emu_get_current_tag());
    NRF_LOG_INFO("ntag generate: %d", err_code);
    if (err_code == NRF_SUCCESS) {
        ntag_emu_set_uuid_only(ntag_emu_get_current_tag());
        mui_update(mui());
    }
}

#ifdef INTERNAL_ENABLE
static void ntag_update(app_amiibolink_t *app, uint8_t index, ntag_t *p_ntag) {
    ret_code_t err_code = ntag_store_write(index, p_ntag);
    NRF_LOG_INFO("ntag_update: index=%d, err_code=%d", index, err_code);
}

static void ntag_reload(app_amiibolink_t *app, uint8_t index){
    ntag_t ntag;
    ret_code_t err_code = ntag_store_read(app->p_amiibolink_view->index, &ntag);
    if (err_code == NRF_SUCCESS) {
        ntag_emu_set_tag(&ntag);
    }
}

static void ntag_init(){ }
#else

static void ntag_update(app_amiibolink_t *app, uint8_t index, ntag_t *p_ntag) {
    char path[VFS_MAX_PATH_LEN] = {0};
    sprintf(path, "/amiibolink/%02d.bin", index);
    vfs_driver_t * p_driver = vfs_get_driver(VFS_DRIVE_EXT);
    int32_t err_code = p_driver->write_file_data(path, p_ntag->data, NTAG_DATA_SIZE);
    NRF_LOG_INFO("ntag_update: index=%d, err_code=%d", index, err_code);
}

static void ntag_reload(app_amiibolink_t *app, uint8_t index){
    ntag_t ntag = {0};
    char path[VFS_MAX_PATH_LEN] = {0};

    memset(&ntag, 0, sizeof(ntag_t));
    memset(path, 0, sizeof(path));
    sprintf(path, "/amiibolink/%02d.bin", index);
    vfs_driver_t * p_driver = vfs_get_driver(VFS_DRIVE_EXT);
    int32_t err_code = p_driver->read_file_data(path, ntag.data, NTAG_DATA_SIZE);
    if (err_code >= NTAG_DATA_SIZE) {
        ntag_emu_set_tag(&ntag);
    }else{
        NRF_LOG_INFO("ntag_reload: ntag_load_failed. index=%d, err_code=%d", index, err_code);
        ntag_store_new_rand(&ntag);
        ntag_emu_set_tag(&ntag);
    }
}

static void ntag_init(){
    vfs_driver_t * p_driver = vfs_get_driver(VFS_DRIVE_EXT);
    p_driver->create_dir("/amiibolink");
}

#endif

static void ntag_update_cb(ntag_event_type_t type, void *context, ntag_t *p_ntag) {

    app_amiibolink_t *app = context;

    if (type == NTAG_EVENT_TYPE_WRITTEN && app->amiibolink_mode != BLE_AMIIBOLINK_MODE_RANDOM) {
        ntag_update(app, app->p_amiibolink_view->index, p_ntag);
        mui_update(mui());
    } else if (type == NTAG_EVENT_TYPE_READ) {
        settings_data_t* p_settings = settings_get_data();
        if (p_settings->auto_gen_amiibolink) {
            app_timer_stop(m_amiibo_gen_delay_timer);
            app_timer_start(m_amiibo_gen_delay_timer, APP_TIMER_TICKS(1000), app);
        }
    }
}

static void amiibolink_scene_main_event_cb(amiibolink_view_event_t event, amiibolink_view_t *p_view) {
    app_amiibolink_t *app = p_view->user_data;
    if (event == AMIIBOLINK_VIEW_EVENT_MENU) {
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, AMIIBOLINK_SCENE_MENU);
    } else if (event == AMIIBOLINK_VIEW_EVENT_UPDATE) {
        ntag_reload(app, app->p_amiibolink_view->index);
    }
}

static void amiibolink_scene_switch_mode(app_amiibolink_t *app, ble_amiibolink_mode_t mode, uint8_t initial_index){
    app->amiibolink_mode = mode;
    amiibolink_view_set_amiibolink_mode(app->p_amiibolink_view, mode);
    amiibolink_view_set_index(app->p_amiibolink_view, initial_index);
    amiibolink_view_set_max_size(app->p_amiibolink_view, MAX_NTAG_INDEX);

    ble_amiibolink_set_mode(mode);

    if(mode == BLE_AMIIBOLINK_MODE_NTAG){
        ntag_reload(app, DEFAULT_NTAG_INDEX);
    }else if(mode == BLE_AMIIBOLINK_MODE_CYCLE){
        ntag_reload(app, initial_index);
    }else if(mode == BLE_AMIIBOLINK_MODE_RANDOM_AUTO_GEN){
        settings_data_t* p_settings = settings_get_data();
        p_settings->auto_gen_amiibolink = true;
    }else if(mode == BLE_AMIIBOLINK_MODE_RANDOM){
        settings_data_t* p_settings = settings_get_data();
        p_settings->auto_gen_amiibolink = false;
    }
}

static void amiibolink_scene_ble_event_handler(void *ctx, ble_amiibolink_event_type_t event_type, uint8_t *data,
                                               size_t size) {
    app_amiibolink_t *app = ctx;
    amiibolink_view_t *p_view = app->p_amiibolink_view;
    if (event_type == BLE_AMIIBOLINK_EVENT_TAG_UPDATED) {

        if (app->amiibolink_mode == BLE_AMIIBOLINK_MODE_CYCLE) {
            // save ntag
            ntag_update(app, p_view->index, (ntag_t *) data);
            if (p_view->index < p_view->max_size - 1) {
                p_view->index++;
            } else {
                p_view->index = 0;
            }
            ntag_reload(app, p_view->index);
        }else if(app->amiibolink_mode == BLE_AMIIBOLINK_MODE_NTAG){
            ntag_update(app, DEFAULT_NTAG_INDEX, (ntag_t *) data);
        }

        mui_update(mui());
    } else if (event_type == BLE_AMIIBOLINK_EVENT_SET_MODE) {
        ble_amiibolink_mode_t mode = *((ble_amiibolink_mode_t *)data);
        amiibolink_scene_switch_mode(app, mode, 0);
        mui_update(mui());
    }
}

void amiibolink_scene_main_on_enter(void *user_data) {
    app_amiibolink_t *app = user_data;

    settings_data_t *p_settings = settings_get_data();

    // enable ble
    ble_init();
    uint8_t device_mode;
    if (p_settings->amiibo_link_ver == BLE_AMIIBOLINK_VER_V2) {
        device_mode = BLE_DEVICE_MODE_AMIIBOLINK_V2;
    } else if (p_settings->amiibo_link_ver == BLE_AMIIBOLINK_VER_V1) {
        device_mode = BLE_DEVICE_MODE_AMIIBOLINK;
    } else {
        device_mode = BLE_DEVICE_MODE_AMILOOP;
    }
    ble_device_mode_prepare(device_mode);
    ble_adv_start();

    ble_amiibolink_set_version(p_settings->amiibo_link_ver);
    ble_nus_set_handler(ble_amiibolink_received_data, NULL);

    ntag_init();
    amiibo_helper_try_load_amiibo_keys_from_vfs();

    ble_amiibolink_init();
    ble_amiibolink_set_event_handler(amiibolink_scene_ble_event_handler, app);

    ntag_emu_set_update_cb(ntag_update_cb, app);

    int32_t err_code = app_timer_create(&m_amiibo_gen_delay_timer, APP_TIMER_MODE_SINGLE_SHOT, ntag_generate_cb);
    APP_ERROR_CHECK(err_code);

    amiibolink_scene_switch_mode(app,app->amiibolink_mode, amiibolink_view_get_index(app->p_amiibolink_view));
    amiibolink_view_set_event_cb(app->p_amiibolink_view, amiibolink_scene_main_event_cb);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, AMIIBOLINK_VIEW_ID_MAIN);
}

void amiibolink_scene_main_on_exit(void *user_data) {
    app_amiibolink_t *app = user_data;

    ble_disable();
    ble_nus_set_handler(NULL, NULL);
    ntag_emu_set_update_cb(NULL, NULL);
}