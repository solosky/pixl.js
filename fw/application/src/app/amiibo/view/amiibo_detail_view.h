#ifndef AMIIBO_DETAIL_VIEW_H
#define AMIIBO_DETAIL_VIEW_H
#include "mui_include.h"
#include <stdint.h>

#include "ntag_def.h"
#include "mui_mlib.h"
#include "m-string.h"


struct amiibo_detail_view_s;
typedef struct amiibo_detail_view_s amiibo_detail_view_t;

typedef enum {
    AMIIBO_DETAIL_VIEW_EVENT_UPDATE,
    AMIIBO_DETAIL_VIEW_EVENT_MENU,
} amiibo_detail_view_event_t;

typedef void (*amiibo_detail_view_event_cb)(amiibo_detail_view_event_t event, amiibo_detail_view_t *p_view);

struct amiibo_detail_view_s {
    mui_view_t *p_view;
    ntag_t *ntag;
    void *user_data;
    uint8_t focus;
    uint8_t max_ntags;
    string_t file_name;
    amiibo_detail_view_event_cb event_cb;
};

amiibo_detail_view_t *amiibo_detail_view_create();
void amiibo_detail_view_free(amiibo_detail_view_t *p_view);
mui_view_t *amiibo_detail_view_get_view(amiibo_detail_view_t *p_view);
void amiibo_detail_view_set_user_data(amiibo_detail_view_t *p_view, void *user_data);
void amiibo_detail_view_set_ntag(amiibo_detail_view_t *p_view, ntag_t *ntag);

inline void amiibo_detail_view_set_event_cb(amiibo_detail_view_t *p_view, amiibo_detail_view_event_cb event_cb) {
    p_view->event_cb = event_cb;
}

inline void amiibo_detail_view_set_focus(amiibo_detail_view_t *p_view, uint8_t focus) { p_view->focus = focus; }

inline uint8_t amiibo_detail_view_get_focus(amiibo_detail_view_t *p_view) { return p_view->focus; }

inline void amiibo_detail_view_set_max_ntags(amiibo_detail_view_t *p_view, uint8_t max_ntags) {
    p_view->max_ntags = max_ntags;
}

inline void amiibo_detail_view_set_file_name(amiibo_detail_view_t *p_view, const char* file_name){
    string_set_str(p_view->file_name, file_name);
}
#endif