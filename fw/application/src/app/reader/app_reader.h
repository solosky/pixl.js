#ifndef APP_READER_H
#define APP_READER_H

#include "mini_app_defines.h"
#include "nfc_scan_view.h"
#include "mui_list_view.h"
#include "nfc_reader.h"

typedef struct {
    nfc_scan_view_t *p_nfc_scan_view;
    mui_list_view_t *p_list_view; 
    mui_view_dispatcher_t *p_view_dispatcher;
    mui_scene_dispatcher_t *p_scene_dispatcher;

    picc_14a_tag_t tag_info;

} app_reader_t;

typedef enum {READER_VIEW_ID_LIST, READER_VIEW_ID_SCAN } reader_view_id_t;

extern mini_app_t app_reader_info;

#endif