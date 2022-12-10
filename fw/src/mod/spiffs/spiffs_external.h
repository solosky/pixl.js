#ifndef SPIFFS_EXTERNAL_H
#define SPIFFS_EXTERNAL_H

#include "nrfx.h"
#include "spiffs.h"


ret_code_t spiffs_external_mount();
spiffs * spiffs_external_get_fs();

#endif 