#ifndef DF_DRIVER_H
#define DF_DRIVER_H

#include "df_defines.h"

typedef struct {
    int32_t (*send_df)(df_frame_t* df); 
    void (*set_df_event_cb)(df_event_callback_t cb);
} df_driver_t;

#endif