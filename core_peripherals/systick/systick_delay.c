#include "pico/stdlib.h"
#include "hardware/structs/sio.h"
#include "hardware/structs/iobank0.h"
#include "stdint.h"

// Direct memory-mapped register pointers for ARM Cortex-M0+ SysTick
#define PPB_BASE          0xe0000000u
#define SYST_CSR          (*(volatile uint32_t *)(PPB_BASE + 0xe010u)) // Control & Status
#define SYST_RVR          (*(volatile uint32_t *)(PPB_BASE + 0xe014u)) // Reload Value
#define SYST_CVR          (*(volatile uint32_t *)(PPB_BASE + 0xe018u)) // Current Value