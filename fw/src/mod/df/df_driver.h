#ifndef DF_DRIVER_H
#define DF_DRIVER_H

#include "df_defines.h"


typedef struct {
    int32_t (*init)(df_event_cb_t cb);
    int32_t (*send)(df_frame_t* df); 
} df_driver_t;

#endif