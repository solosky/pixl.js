#ifndef APP_AMIIDB_H
#define APP_AMIIDB_H

#include "mini_app_defines.h"
#include "mui_list_view.h"
#include "mui_msg_box.h"
#include "mui_scene_dispatcher.h"
#include "mui_text_input.h"
#include "ntag_def.h"
#include "vfs.h"
#include "amiibo_view.h"

#include "mlib_common.h"
#include "db_header.h"


typedef struct {
    mui_list_view_t *p_list_view;
    mui_text_input_t *p_text_input;
    mui_msg_box_t *p_msg_box;
    amiibo_view_t * p_amiibo_view;
    mui_view_dispatcher_t *p_view_dispatcher;
    mui_scene_dispatcher_t *p_scene_dispatcher;
    ntag_t ntag;
    
    /** file browser*/
    const db_amiibo_t * cur_amiibo;
    uint16_t game_id_path[8];
    uint8_t game_id_index;

    string_t cur_fav_dir;
    string_t cur_fav_file;

} app_amiidb_t;

typedef enum {
    AMIIDB_VIEW_ID_LIST,
    AMIIDB_VIEW_ID_DETAIL,
    AMIIDB_VIEW_ID_INPUT,
    AMIIDB_VIEW_ID_MSG_BOX,
} amiidb_view_id_t;

typedef struct {
    bool cached_enabled;
    vfs_drive_t current_drive;
    char current_folder[128];
    char current_file[64];
    uint32_t current_focus_index;
    uint32_t current_scene_id;
} app_amiidb_cache_data_t;


extern const mini_app_t app_amiidb_info;

#endif