#ifndef MUI_DEFINES_H
#define MUI_DEFINES_H


// mlib defines 
#include "mui_conf.h"
#include "mui_mlib.h"


#define MUI_EVENT_ID_REDRAW 0
#define MUI_EVENT_ID_INPUT 1
#define MUI_EVENT_ID_VIEW_ENTER 2
#define MUI_EVENT_ID VIEW_EXIT 3


// #ifndef MAX
// #define MAX(a, b)               \
//     ({                          \
//         __typeof__(a) _a = (a); \
//         __typeof__(b) _b = (b); \
//         _a > _b ? _a : _b;      \
//     })
// #endif

// #ifndef MIN
// #define MIN(a, b)               \
//     ({                          \
//         __typeof__(a) _a = (a); \
//         __typeof__(b) _b = (b); \
//         _a < _b ? _a : _b;      \
//     })
// #endif


#define mui_check(__e)                          \
    do {                                         \
        if((__e) == 0) {                         \
            NRF_LOG_INFO("furi_check failed\r\n"); \
        }                                        \
    } while(0)

#define mui_assert(__e) \
    do {                 \
        ((void)(__e));   \
    } while(0)

#endif