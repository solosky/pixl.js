#ifndef DF_LINK_H
#define DF_LINK_H

#include "df_defines.h"

typedef struct {
    int32_t (*send_data_frame)(df_frame_t* df); 
    void (*set_data_frame_event_cb)(df_event_callback_t cb);
} df_link_driver_t;

#endif