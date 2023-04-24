#ifndef APP_AMIIBO_H
#define APP_AMIIBO_H

#include "amiibo_detail_view.h"
#include "mini_app_defines.h"
#include "mui_list_view.h"
#include "mui_msg_box.h"
#include "mui_scene_dispatcher.h"
#include "mui_text_input.h"
#include "ntag_def.h"
#include "vfs.h"

#include "mlib_common.h"


typedef struct {
    amiibo_detail_view_t *p_amiibo_detail_view;
    mui_list_view_t *p_list_view;
    mui_text_input_t *p_text_input;
    mui_msg_box_t *p_msg_box;
    mui_view_dispatcher_t *p_view_dispatcher;
    mui_scene_dispatcher_t *p_scene_dispatcher;
    ntag_t ntag;
    
    /** file browser*/
    vfs_drive_t current_drive;
    string_t current_folder;
    string_t current_file;

    /**amiibo detail view*/
    string_array_t amiibo_files;
    bool reload_amiibo_files;

} app_amiibo_t;

typedef struct {
    vfs_drive_t current_drive;
    char current_folder[128];
    char current_file[64];
} app_amiibo_cache_data_t;


typedef enum {
    AMIIBO_VIEW_ID_LIST,
    AMIIBO_VIEW_ID_DETAIL,
    AMIIBO_VIEW_ID_INPUT,
    AMIIBO_VIEW_ID_MSG_BOX,
} amiibo_view_id_t;

extern const mini_app_t app_amiibo_info;

#endif