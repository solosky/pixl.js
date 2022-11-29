#ifndef MUI_LIST_VIEW_H
#define MUI_LIST_VIEW_H


#include "mui_include.h"
#include <stdint.h>
#include "m-array.h"
#include "m-string.h"

struct mui_list_item_s;
typedef struct mui_list_item_s mui_list_item_t;

typedef void (*mui_list_view_selected_cb)(mui_list_item_t* p_item);

struct mui_list_item_s{
    uint16_t icon;
    string_t text;
    void* user_data;
} ;

ARRAY_DEF(mui_list_item_array, mui_list_item_t, M_POD_OPLIST);


typedef struct {
    mui_view_t* p_view;
    uint32_t focus_index;
    mui_list_view_selected_cb selcted_cb;
    mui_list_item_array_t items;
    void* user_data;
} mui_list_view_t;

mui_list_view_t* mui_list_view_create();
void mui_list_view_free(mui_list_view_t* p_view);
mui_view_t* mui_list_view_get_view(mui_list_view_t* p_view);
void mui_list_view_set_user_data(mui_list_view_t* p_view, void* user_data);

//// view functions //
void mui_list_view_add_item(mui_list_view_t* p_view, uint32_t icon, char* text, void* user_data);
void mui_list_view_set_selected_cb(mui_list_view_t* p_view, mui_list_view_selected_cb selected_cb);

#endif