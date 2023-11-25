#include "settings.h"
#include "nrf_error.h"
#include "nrf_log.h"
#include "vfs.h"
#include "vfs_meta.h"

#define SETTINGS_FILE_NAME "/settings.bin"

const settings_data_t def_settings_data = {.backlight = 0,
                                           .oled_contrast = 40,
                                           .auto_gen_amiibo = 0,
                                           .auto_gen_amiibolink = 0,
                                           .sleep_timeout_sec = 30,
                                           .skip_driver_select = 0,
                                           .bat_mode = 0,
                                           .amiibo_link_ver = BLE_AMIIBOLINK_VER_V1,
                                           .language = LANGUAGE_ZH_HANS,
                                           .hibernate_enabled = false,
                                           .show_mem_usage = false,
                                           .anim_enabled = false,
                                           .amiidb_data_slot_num = 20,
                                           .qrcode_enabled = true};

settings_data_t m_settings_data = {0};

#define BOOL_VALIDATE(expr, default_val)                                                                               \
    if ((expr) != 0 && (expr) != 1) {                                                                                  \
        (expr) = (default_val);                                                                                        \
    }
#define INT8_VALIDATE(expr, min, max, default_val)                                                                     \
    if ((expr) < (min) || (expr) > (max)) {                                                                            \
        (expr) = (default_val);                                                                                        \
    }

static void validate_settings() {
    if (m_settings_data.sleep_timeout_sec == 0 || m_settings_data.sleep_timeout_sec > 180) {
        m_settings_data.sleep_timeout_sec = 30;
    }

    if (m_settings_data.amiibo_link_ver != BLE_AMIIBOLINK_VER_V1 &&
        m_settings_data.amiibo_link_ver != BLE_AMIIBOLINK_VER_V2 && m_settings_data.amiibo_link_ver != BLE_AMILOOP) {
        m_settings_data.amiibo_link_ver = BLE_AMIIBOLINK_VER_V1;
    }

    BOOL_VALIDATE(m_settings_data.skip_driver_select, 0);
    BOOL_VALIDATE(m_settings_data.show_mem_usage, 0);
    BOOL_VALIDATE(m_settings_data.hibernate_enabled, 0);
    BOOL_VALIDATE(m_settings_data.bat_mode, 0);
    BOOL_VALIDATE(m_settings_data.skip_driver_select, 0);
    BOOL_VALIDATE(m_settings_data.auto_gen_amiibo, 0);
    BOOL_VALIDATE(m_settings_data.auto_gen_amiibolink, 0);
    BOOL_VALIDATE(m_settings_data.backlight, 0);
    INT8_VALIDATE(m_settings_data.lcd_backlight, 0, 100, 0);
    INT8_VALIDATE(m_settings_data.oled_contrast, 0, 100, 0);
    BOOL_VALIDATE(m_settings_data.anim_enabled, 0);
    BOOL_VALIDATE(m_settings_data.qrcode_enabled, 0);
    INT8_VALIDATE(m_settings_data.language, 0, LANGUAGE_COUNT - 1, 0);
    INT8_VALIDATE(m_settings_data.amiidb_data_slot_num, 1, 100, 20);
}

int32_t settings_init() {
    memcpy(&m_settings_data, &def_settings_data, sizeof(settings_data_t));
    vfs_driver_t *p_driver = vfs_get_default_driver();
    if (p_driver == NULL) {
        return NRF_ERROR_NOT_SUPPORTED;
    }
    int32_t err = 0;
    if (!p_driver->mounted()) {
        err = p_driver->mount();
    }

    if (!p_driver->mounted()) {
        return NRF_ERROR_INVALID_STATE;
    }

    err = p_driver->read_file_data(SETTINGS_FILE_NAME, &m_settings_data, sizeof(settings_data_t));
    if (err < 0) {
        return NRF_ERROR_INVALID_STATE;
    }

    validate_settings();

    NRF_LOG_INFO("settings loaded!");
    return NRF_SUCCESS;
}

int32_t settings_save() {
    vfs_driver_t *p_driver = vfs_get_default_driver();
    int32_t err;

    if (p_driver == NULL) {
        return NRF_ERROR_NOT_SUPPORTED;
    }

    settings_data_t old_settings_data;

    err = p_driver->read_file_data(SETTINGS_FILE_NAME, &old_settings_data, sizeof(settings_data_t));
    bool not_found = false;
    if (err == VFS_ERR_NOOBJ) {
        not_found = true;
    } else if (err < 0) {
        return NRF_ERROR_INVALID_STATE;
    }

    if (not_found || memcmp(&m_settings_data, &old_settings_data, sizeof(settings_data_t)) != 0) {
        err = p_driver->write_file_data(SETTINGS_FILE_NAME, &m_settings_data, sizeof(settings_data_t));
        if (err < 0) {
            return NRF_ERROR_INVALID_STATE;
        }

        if (not_found) {
            vfs_meta_t meta;
            memset(&meta, 0, sizeof(meta));
            meta.has_flags = true;
            meta.flags = VFS_OBJ_FLAG_HIDDEN;

            uint8_t meta_data[VFS_MAX_META_LEN];
            vfs_meta_encode(meta_data, sizeof(meta_data), &meta);
            err = p_driver->update_file_meta(SETTINGS_FILE_NAME, &meta_data, sizeof(meta_data));
            NRF_LOG_INFO("Settings file meta updated!: %d", err);
        }

        NRF_LOG_INFO("settings saved!");
    }

    return NRF_SUCCESS;
}

settings_data_t *settings_get_data() { return &m_settings_data; }

int32_t settings_reset() {
    memcpy(&m_settings_data, &def_settings_data, sizeof(settings_data_t));
    vfs_driver_t *p_driver = vfs_get_default_driver();
    return p_driver->remove_file(SETTINGS_FILE_NAME);
}