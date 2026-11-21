#ifndef APP_SWITCH2_WAKE_H
#define APP_SWITCH2_WAKE_H

#include <stdint.h>
#include "mini_app_defines.h"

extern const mini_app_t app_switch2_wake_info;

void app_switch2_wake_on_boot(uint32_t wakeup_reason);

#endif /* APP_SWITCH2_WAKE_H */
