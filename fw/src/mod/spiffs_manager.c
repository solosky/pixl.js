#include "spiffs_manager.h"
#include "hal_internal_flash.h"
#include "nrf_log.h"

typedef struct {
    bool avaliable;
    spiffs *fs;
} spiffs_state_t;

static spiffs_state_t fs_state[SPIFFS_DRIVE_MAX] = {0};

bool spiffs_man_fs_avaliable(spiffs_drive_t drive) { return fs_state[drive].avaliable; }

void spiffs_man_mount_drives() {
    ret_code_t err;

    err = spiffs_internal_mount();

    NRF_LOG_INFO("spiffs internal drive mount: %d", err);
    if (!err) {
        fs_state[SPIFFS_DRIVE_INTERNAL].avaliable = true;
        fs_state[SPIFFS_DRIVE_INTERNAL].fs = spiffs_internal_get_fs();
    }
}

spiffs *spiffs_man_get_fs(spiffs_drive_t drive) {
    if (fs_state[drive].avaliable) {
        return fs_state[drive].fs;
    }
    return NULL;
}