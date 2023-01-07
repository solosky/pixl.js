#ifndef STATUS_BAR_VIEW_H
#define STATUS_BAR_VIEW_H

#include "mui_include.h"


struct ble_status_view_s;
typedef struct ble_status_view_s ble_status_view_t;

typedef enum {
    BLE_STATUS_VIEW_BLE_DISABLE,
} ble_status_view_event_t;

typedef void (*ble_status_view_event_cb)(ble_status_view_event_t event, ble_status_view_t *p_view);

 struct ble_status_view_s{
    mui_view_t* p_view;
    ble_status_view_event_cb event_cb;
} ;

ble_status_view_t* ble_status_view_create();
void ble_status_view_free(ble_status_view_t* p_view);
mui_view_t* ble_status_view_get_view(ble_status_view_t* p_view);

static inline void ble_status_view_set_event_cb(ble_status_view_t* p_view, ble_status_view_event_cb event_cb){
    p_view->event_cb = event_cb;
}


#endif