#ifndef MINI_APP_DEFINES_H
#define MINI_APP_DEFINES_H

struct mini_app_inst_s;
typedef struct mini_app_inst_s mini_app_inst_t;

struct mini_app_s;
typedef struct mini_app_s mini_app_t;


typedef void (* mini_app_launch_cb_t)(mini_app_inst_t* p_app_inst);
typedef void (* mini_app_kill_cb_t)(mini_app_inst_t* p_app_inst);


typedef enum {
    RUNNING,
    NOT_RUNNING
} mini_app_state_t;


struct mini_app_inst_s {
    const mini_app_t* p_app;
    mini_app_state_t state;
    void* p_handle;
    void* p_context;
};


#endif