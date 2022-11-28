#ifndef AMIIBO_VIEW_H
#define AMIIBO_VIEW_H
#include "mui_include.h"
#include <stdint.h>


typedef struct {
    mui_view_t* p_view;
    uint8_t focus_index;
    char amiibo_name[6][32];
} amiibo_view_t;

amiibo_view_t* amiibo_view_create();
void amiibo_view_free(amiibo_view_t* p_view);
mui_view_t* amiibo_view_get_view(amiibo_view_t* p_view);


#endif