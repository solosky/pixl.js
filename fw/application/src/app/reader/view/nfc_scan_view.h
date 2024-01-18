#ifndef NFC_SCAN_VIEW_H
#define NFC_SCAN_VIEW_H

#include "mui_include.h"


struct nfc_scan_view_s;
typedef struct nfc_scan_view_s nfc_scan_view_t;

typedef enum {
    NFC_SCAN_VIEW_MENU,
} nfc_scan_view_event_t;

typedef void (*nfc_scan_view_event_cb)(nfc_scan_view_event_t event, nfc_scan_view_t *p_view);

 struct nfc_scan_view_s{
    mui_view_t* p_view;
    nfc_scan_view_event_cb event_cb;
    void* user_data;
} ;

nfc_scan_view_t* nfc_scan_view_create();
void nfc_scan_view_free(nfc_scan_view_t* p_view);
mui_view_t* nfc_scan_view_get_view(nfc_scan_view_t* p_view);

static inline void nfc_scan_view_set_event_cb(nfc_scan_view_t* p_view, nfc_scan_view_event_cb event_cb){
    p_view->event_cb = event_cb;
}
static inline void nfc_scan_view_set_user_data(nfc_scan_view_t* p_view, void* user_data){
    p_view->user_data = user_data;
}

#endif