#ifndef SPIFFS_INTERNAL_H
#define SPIFFS_INTERNAL_H

#include "nrfx.h"
#include "spiffs.h"


ret_code_t spiffs_internal_mount();
spiffs * spiffs_internal_get_fs();

#endif 