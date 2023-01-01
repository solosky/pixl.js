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
    AMIIBO_DETAIL_VIEW_EVENT_PREV,
    AMIIBO_DETAIL_VIEW_EVENT_NEXT,
    AMIIBO_DETAIL_VIEW_EVENT_MENU,
} amiibo_detail_view_event_t;

typedef void (* amiibo_detail_view_event_cb)(amiibo_detail_view_event_t event, amiibo_detail_view_t* p_view);

struct amiibo_detail_view_s {
    mui_view_t* p_view;
    ntag_t * ntag;
    void* user_data;
    amiibo_detail_view_event_cb event_cb;
} ;

amiibo_detail_view_t* amiibo_detail_view_create();
void amiibo_detail_view_free(amiibo_detail_view_t* p_view);
mui_view_t* amiibo_detail_view_get_view(amiibo_detail_view_t* p_view);


#endif