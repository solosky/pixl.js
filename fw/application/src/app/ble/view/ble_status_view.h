#ifndef STATUS_BAR_VIEW_H
#define STATUS_BAR_VIEW_H

#include "mui_include.h"


struct ble_status_view_s;
typedef struct ble_status_view_s ble_status_view_t;

typedef enum {
    BLE_STATUS_VIEW_BLE_DISABLE,
    BLE_STATUS_VIEW_EVENT_UPDATE,
} ble_status_view_event_t;

typedef void (*ble_status_view_event_cb)(ble_status_view_event_t event, ble_status_view_t *p_view);

 struct ble_status_view_s{
    mui_view_t* p_view;
    ble_status_view_event_cb event_cb;
    char ble_addr[20];
    uint8_t page;
    void* user_data;
} ;

ble_status_view_t* ble_status_view_create();
void ble_status_view_free(ble_status_view_t* p_view);
mui_view_t* ble_status_view_get_view(ble_status_view_t* p_view);

static inline void ble_status_view_set_event_cb(ble_status_view_t* p_view, ble_status_view_event_cb event_cb){
    p_view->event_cb = event_cb;
}

static char* ble_status_view_get_ble_addr(ble_status_view_t* p_view){
    return p_view->ble_addr;
}

static inline void ble_status_view_set_user_data(ble_status_view_t* p_view, void* user_data){
    p_view->user_data = user_data;
}

inline void ble_status_view_set_page(ble_status_view_t *p_view, uint8_t page) { p_view->page = page; }

inline uint8_t ble_status_view_get_page(ble_status_view_t *p_view) { return p_view->page; }

#endif