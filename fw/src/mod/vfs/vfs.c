#include "vfs.h"

#ifdef VFS_LFS_ENABLE
    #include "vfs_driver_lfs.h"
    #define VFS_DRIVER {.enabled = true, .p_driver = &vfs_driver_lfs}
#else
    #include "vfs_driver_spiffs.h"
    #define VFS_DRIVER {.enabled = true, .p_driver = &vfs_driver_spiffs}
#endif

typedef struct {
    bool enabled;
    vfs_driver_t *p_driver;
} vfs_drive_item_t;

static vfs_drive_item_t vfs_drive_items[VFS_DRIVE_MAX] = {
    [VFS_DRIVE_INT] =
        {
            .enabled = false,
            .p_driver = NULL,
        },
    [VFS_DRIVE_EXT] = VFS_DRIVER
};


bool vfs_drive_enabled(vfs_drive_t drive) { return vfs_drive_items[drive].enabled; }

vfs_driver_t *vfs_get_driver(vfs_drive_t drive) { return vfs_drive_items[drive].p_driver; }

vfs_driver_t* vfs_get_default_driver() {
    for(uint32_t i=0; i<VFS_DRIVE_MAX; i++ ){
        if(vfs_drive_items[i].enabled){
            return vfs_drive_items[i].p_driver;
        }
    }
    return NULL;
}

vfs_drive_t vfs_get_default_drive() {
    for(uint32_t i=0; i<VFS_DRIVE_MAX; i++ ){
        if(vfs_drive_items[i].enabled){
            return i;
        }
    }
    return VFS_DRIVE_MAX;
}