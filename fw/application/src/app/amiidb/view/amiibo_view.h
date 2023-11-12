#ifndef AMIIBO_DETAIL_VIEW2_H
#define AMIIBO_DETAIL_VIEW2_H
#include "mui_include.h"
#include <stdint.h>

#include "ntag_def.h"
#include "mui_mlib.h"
#include "m-string.h"


struct amiibo_view_s;
typedef struct amiibo_view_s amiibo_view_t;

typedef enum {
    AMIIBO_VIEW_EVENT_UPDATE,
    AMIIBO_VIEW_EVENT_DESC_UPDATE,
    AMIIBO_VIEW_EVENT_MENU,
} amiibo_view_event_t;

typedef void (*amiibo_view_event_cb)(amiibo_view_event_t event, amiibo_view_t *p_view);

struct amiibo_view_s {
    mui_view_t *p_view;
    ntag_t *ntag;
    void *user_data;
    uint8_t focus;
    uint8_t max_ntags;
    uint8_t game_id;
    amiibo_view_event_cb event_cb;
    uint16_t desc_offset;
};

amiibo_view_t *amiibo_view_create();
void amiibo_view_free(amiibo_view_t *p_view);
mui_view_t *amiibo_view_get_view(amiibo_view_t *p_view);
inline void amiibo_view_set_user_data(amiibo_view_t *p_view, void *user_data) { p_view->user_data = user_data; }
inline void amiibo_view_set_ntag(amiibo_view_t *p_view, ntag_t *ntag) { p_view->ntag = ntag; }

inline void amiibo_view_set_event_cb(amiibo_view_t *p_view, amiibo_view_event_cb event_cb) {
    p_view->event_cb = event_cb;
}

inline void amiibo_view_set_focus(amiibo_view_t *p_view, uint8_t focus) { p_view->focus = focus; }

inline uint8_t amiibo_view_get_focus(amiibo_view_t *p_view) { return p_view->focus; }

inline void amiibo_view_set_max_ntags(amiibo_view_t *p_view, uint8_t max_ntags) {
    p_view->max_ntags = max_ntags;
}

inline void amiibo_view_set_game_id(amiibo_view_t *p_view, uint16_t game_id){
    p_view->game_id = game_id;
}
#endif