#include "spiffs_manager.h"
#include "spiffs_internal.h"
#include "spiffs_external.h"
#include "nrf_log.h"

typedef struct {
    bool avaliable;
    spiffs *fs;
} spiffs_state_t;

static spiffs_state_t fs_state[SPIFFS_DRIVE_MAX] = {0};

bool spiffs_man_fs_avaliable(spiffs_drive_t drive) { return fs_state[drive].avaliable; }

void spiffs_man_mount_drives() {
    ret_code_t err;
    u32_t total, used;

    err = spiffs_internal_mount();

    NRF_LOG_INFO("spiffs internal drive mount: %d", err);
    if (!err) {
        fs_state[SPIFFS_DRIVE_INTERNAL].avaliable = true;
        fs_state[SPIFFS_DRIVE_INTERNAL].fs = spiffs_internal_get_fs();
    }

    err = spiffs_external_mount();

    NRF_LOG_INFO("spiffs external drive mount: %d", err);
    if (!err) {
        fs_state[SPIFFS_DRIVE_EXTERNAL].avaliable = true;
        fs_state[SPIFFS_DRIVE_EXTERNAL].fs = spiffs_external_get_fs();

        SPIFFS_info(spiffs_external_get_fs(), &total, &used);
        NRF_LOG_INFO("external dirve stat: total = %d bytes, used = %d bytes", total, used);
    }

    
}

spiffs *spiffs_man_get_fs(spiffs_drive_t drive) {
    if (fs_state[drive].avaliable) {
        return fs_state[drive].fs;
    }
    return NULL;
}