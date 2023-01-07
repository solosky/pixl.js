#ifndef CMD_CORE_H
#define CMD_CORE_H

typedef void (*cmd_handler_t)();

typedef struct {
    uint8_t cmd;
    cmd_handler_t handler;
} cmd_registry_t;
#endif