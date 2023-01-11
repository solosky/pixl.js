#ifndef DF_CORE_H
#define DF_CORE_H

#include "df_defines.h"
#include "df_driver.h"

struct df_core_s;
typedef struct df_core_s df_core_t;


typedef void(*df_proto_handler_t)(df_core_t* p_core, df_frame_t* p_frame);

typedef struct {
    uint8_t cmd;
    df_proto_handler_t handler;
} df_proto_handler_entry_t;


struct df_core_s {
    df_driver_t* p_driver;
};


#endif