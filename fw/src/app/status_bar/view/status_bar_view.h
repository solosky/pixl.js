#ifndef STATUS_BAR_VIEW_H
#define STATUS_BAR_VIEW_H

#include "mui_include.h"

typedef struct {
    mui_view_t* p_view;

} status_bar_view_t;

status_bar_view_t* status_bar_view_create();
void status_bar_view_free(status_bar_view_t* p_view);
mui_view_t* status_bar_view_get_view(status_bar_view_t* p_view);


#endif