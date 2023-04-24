#ifndef MINI_APP_DEFINES_H
#define MINI_APP_DEFINES_H

#include <stdint.h>

struct mini_app_inst_s;
typedef struct mini_app_inst_s mini_app_inst_t;

struct mini_app_s;
typedef struct mini_app_s mini_app_t;

typedef enum {
    WEAKUP_EVENT,
    DORMANCY_EVENT
} mini_app_event_stats_t;

typedef struct {
    mini_app_event_stats_t event_id;
    void* data;
} mini_app_event_t;


typedef void (* mini_app_run_cb_t)(mini_app_inst_t* p_app_inst);
typedef void (* mini_app_kill_cb_t)(mini_app_inst_t* p_app_inst);
typedef void (* mini_app_post_event_cb_t)(mini_app_inst_t* p_app_inst, mini_app_event_t* p_event);


typedef enum {
    APP_STATE_RUNNING,
    APP_STATE_KILLED
} mini_app_state_t;


struct mini_app_inst_s {
    const mini_app_t* p_app;
    mini_app_state_t state;
    void* p_handle;
};


struct mini_app_s {
    uint32_t id;
    char name[16];
    uint16_t icon;
    uint8_t deamon;
    uint8_t sys;
    mini_app_run_cb_t run_cb;
    mini_app_kill_cb_t kill_cb;
    mini_app_post_event_cb_t on_event_cb;
};

#endif