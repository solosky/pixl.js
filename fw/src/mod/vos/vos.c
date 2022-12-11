#include "vos.h"
#include "vos_driver_spiffs.h"

typedef struct {
    bool enabled;
    vos_driver_t *p_driver;
} vos_drive_item_t;

static vos_drive_item_t vos_drive_items[VOS_DRIVE_MAX] = {
    [VOS_DRIVE_INT] =
        {
            .enabled = false,
            .p_driver = NULL,
        },
    [VOS_DRIVE_EXT] = {.enabled = true, .p_driver = &vos_driver_spiffs}};


bool vos_drive_enabled(vos_drive_t drive) { return vos_drive_items[drive].enabled; }

vos_driver_t *vos_get_driver(vos_drive_t drive) { return vos_drive_items[drive].p_driver; }