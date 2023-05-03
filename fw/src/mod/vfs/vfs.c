#include "vfs.h"
#include "hal_spi_flash.h"
#include "vfs_driver_spiffs.h"

static vfs_drive_item_t vfs_drive_items[VFS_DRIVE_MAX] = {
    [VFS_DRIVE_INT] =
        {
            .enabled = false,
            .p_driver = NULL,
        },
    [VFS_DRIVE_EXT] = {.enabled = true, .p_driver = NULL}};


bool vfs_drive_enabled(vfs_drive_t drive) { return vfs_drive_items[drive].enabled; }

vfs_driver_t *vfs_get_driver(vfs_drive_t drive) { 
    if (vfs_drive_items[drive].enabled && vfs_drive_items[drive].p_driver == NULL) {
        int32_t err_code = hal_spi_flash_init();
        if (err_code) {
            return NULL;
        }
        hal_spi_flash_driver(&(vfs_drive_items[drive]));
    }
    return vfs_drive_items[drive].p_driver;
}

vfs_drive_type_t vfs_get_type(vfs_drive_t drive) {
    return vfs_drive_items[drive].type;
}