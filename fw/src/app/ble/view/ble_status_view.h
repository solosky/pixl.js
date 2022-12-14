#ifndef STATUS_BAR_VIEW_H
#define STATUS_BAR_VIEW_H

#include "mui_include.h"

typedef struct {
    mui_view_t* p_view;

} ble_status_view_t;

ble_status_view_t* ble_status_view_create();
void ble_status_view_free(ble_status_view_t* p_view);
mui_view_t* ble_status_view_get_view(ble_status_view_t* p_view);


#endif