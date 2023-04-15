#include "cache.h"

#include "ntag_emu.h"

#include "nrf_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "vfs.h"
#include "vfs_meta.h"

#include "nrf_pwr_mgmt.h"
#include "settings.h"

#define CACHE_FILE_NAME "/cache.bin"

cache_data_t m_cache_data = {.enabled = false, .current_file = "", .current_folder = "", .current_drive = VFS_DRIVE_MAX};

bool load = false;

static vfs_driver_t *get_enabled_vfs_driver() {
    if (vfs_drive_enabled(VFS_DRIVE_EXT)) {
        return vfs_get_driver(VFS_DRIVE_EXT);
    } else if (vfs_drive_enabled(VFS_DRIVE_INT)) {
        return vfs_get_driver(VFS_DRIVE_INT);
    }

    return NULL;
}

int32_t cache_init() {
    if (load) {
        return NRF_SUCCESS;
    }
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

    err = p_driver->read_file_data(CACHE_FILE_NAME, &m_cache_data, sizeof(cache_data_t));
    if (err < 0) {
        return NRF_ERROR_INVALID_STATE;
    }

    NRF_LOG_INFO("Cache loaded!");
    if (m_cache_data.current_file == "" || m_cache_data.current_folder == "" || m_cache_data.current_drive == VFS_DRIVE_MAX) {
        m_cache_data.enabled = false;
    }
    NRF_LOG_INFO("Cache: enabled = %d, current_folder = %s, current_file = %s, current_drive = %d", m_cache_data.enabled, m_cache_data.current_folder, m_cache_data.current_file, m_cache_data.current_drive);
    load = true;
    return NRF_SUCCESS;
}

int32_t cache_clean() {
    NRF_LOG_INFO("Cleaning cache...")
    m_cache_data = (cache_data_t){.enabled = false, .current_file = "", .current_folder = "", .current_drive = VFS_DRIVE_MAX};
    cache_save();

    return NRF_SUCCESS;
}

bool lcdled = false;
void weak_up_set_lcdled(bool on) {
    if (lcdled) {
        return;
    }
    if (on) {
        nrf_pwr_mgmt_set_timeout(5);
    } else {
        lcdled = true;
        nrf_pwr_mgmt_set_timeout(settings_get_data()->sleep_timeout_sec);
    }
}

int32_t cache_save() {
    NRF_LOG_INFO("Saving cache...");
    NRF_LOG_INFO("Cache: %d, %s, %s, %d", m_cache_data.enabled, m_cache_data.current_folder, m_cache_data.current_file, m_cache_data.current_drive);

    vfs_driver_t *p_driver = get_enabled_vfs_driver();
    int32_t err;

    if (p_driver == NULL) {
        return NRF_ERROR_NOT_SUPPORTED;
    }

    cache_data_t old_cache_data;

    err = p_driver->read_file_data(CACHE_FILE_NAME, &old_cache_data, sizeof(cache_data_t));
    bool not_found = false;
    if (err == VFS_ERR_NOOBJ) {
        not_found = true;
    } else if (err < 0) {
        NRF_LOG_ERROR("Error reading cache file #1: %d", err);
        return NRF_ERROR_INVALID_STATE;
    }

    if (m_cache_data.current_drive != VFS_DRIVE_MAX && m_cache_data.current_folder != "" && m_cache_data.current_file != "") {
        m_cache_data.enabled = true;
    } else {
        m_cache_data.enabled = false;
    }

    err = p_driver->write_file_data(CACHE_FILE_NAME, &m_cache_data, sizeof(cache_data_t));
    if (err < 0) {
        NRF_LOG_ERROR("Error writing cache file #2: %d", err);
        return NRF_ERROR_INVALID_STATE;
    }

    if (not_found) {
        vfs_meta_t meta;
        memset(&meta, 0, sizeof(meta));
        meta.has_flags = true;
        meta.flags = VFS_OBJ_FLAG_HIDDEN;

        uint8_t meta_data[VFS_MAX_META_LEN];
        vfs_meta_encode(meta_data, sizeof(meta_data), &meta);
        err = p_driver->update_file_meta(CACHE_FILE_NAME, &meta_data, sizeof(meta_data));
        NRF_LOG_INFO("Cache file meta updated!: %d", err);
    }

    NRF_LOG_INFO("Cache saved!");

    return NRF_SUCCESS;
}

cache_data_t *cache_get_data() {
    if (!load) {
        cache_init();
    }
    return &m_cache_data;
}