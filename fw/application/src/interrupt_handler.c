#include "nrf_nvic.h"

#define DEF_INTERRUPT_HANDLER(name)                                                                                    \
    void name(void) { NVIC_SystemReset(); }

// DEF_INTERRUPT_HANDLER(HardFault_Handler)
DEF_INTERRUPT_HANDLER(MemoryManagement_Handler)
DEF_INTERRUPT_HANDLER(BusFault_Handler)
DEF_INTERRUPT_HANDLER(UsageFault_Handler)
DEF_INTERRUPT_HANDLER(DebugMon_Handler)