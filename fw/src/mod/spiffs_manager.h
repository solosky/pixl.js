#ifndef SPIFFS_MANAGER_H
#define SPIFFS_MANAGER_H

#include "spiffs.h"
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    SPIFFS_DRIVE_INTERNAL,
    SPIFFS_DRIVE_EXTERNAL,
    SPIFFS_DRIVE_MAX
} spiffs_drive_t;

bool spiffs_man_fs_avaliable(spiffs_drive_t drive);
void spiffs_man_mount_drives();
spiffs *spiffs_man_get_fs(spiffs_drive_t drive);

#endif