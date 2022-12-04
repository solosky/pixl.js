#ifndef AMIIBO_VIEW_H
#define AMIIBO_VIEW_H
#include "mui_include.h"
#include <stdint.h>
#include "m-string.h"
#include "ntag_def.h"

struct amiibo_view_s;
typedef struct amiibo_view_s amiibo_view_t;

typedef void (* amiibo_view_close_cb)(amiibo_view_t* p_view);

struct amiibo_view_s {
    mui_view_t* p_view;
    ntag_t * amiibo;
    void* user_data;
    amiibo_view_close_cb on_close_cb;
} ;

amiibo_view_t* amiibo_view_create();
void amiibo_view_free(amiibo_view_t* p_view);
mui_view_t* amiibo_view_get_view(amiibo_view_t* p_view);


#endif