#include "settings.h"
#include "nrf_error.h"
#include "nrf_log.h"
#include "vfs.h"

#define SETTINGS_FILE_NAME "/settings.bin"

settings_data_t m_settings_data = {.backlight = 0, .sleep_timeout_sec = 30};

static vfs_driver_t *get_enabled_vfs_driver() {
    if (vfs_drive_enabled(VFS_DRIVE_EXT)) {
        return vfs_get_driver(VFS_DRIVE_EXT);
    } else if (vfs_drive_enabled(VFS_DRIVE_INT)) {
        return vfs_get_driver(VFS_DRIVE_INT);
    }

    return NULL;
}

static void validate_settings() {
    if (m_settings_data.sleep_timeout_sec == 0 || m_settings_data.sleep_timeout_sec > 180) {
        m_settings_data.sleep_timeout_sec = 30;
    }
}

int32_t settings_init() {
    vfs_driver_t *p_driver = get_enabled_vfs_driver();
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
    vfs_driver_t *p_driver = get_enabled_vfs_driver();
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

        NRF_LOG_INFO("settings saved!");
    }

    return NRF_SUCCESS;
}

settings_data_t *settings_get_data() { return &m_settings_data; }