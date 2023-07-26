#ifndef MUI_ANIM_H
#define MUI_ANIM_H

#include <stdint.h>

typedef void (*mui_anim_exec_cb_t)(void* var, int32_t value);
typedef int32_t (*mui_anim_path_cb_t)(int32_t begin, int32_t end, int32_t current);

typedef struct {
    void* var;
    int32_t begin;
    int32_t end;
    int32_t current;
    mui_anim_exec_cb_t exec_cb;
    mui_anim_path_cb_t path_cb;
    uint32_t time; //in ms
} mui_anim_t;


#endif 