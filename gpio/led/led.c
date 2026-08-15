
#include "pico/stdlib.h"
#include "hardware/structs/sio.h"
#include "hardware/structs/iobank0.h"
#include "stdint.h"

/* Definition of peripherals address's map:
    .pico-sdk/sdk/2.3.0/src/rp2040/hardware_regs/include/hardware/regs/addressmap.h*/
#define PICO_IO_BANK0_BASE 0x40014000
#define PICO_IOBANK_GPIO_CTRL     *(volatile uint32_t *)(IO_BANK0_BASE + 0x0cc) //GPIO25 CTRL reg (0x400140cc)
#define PICO_SIO_BASE      0xd0000000
#define PICO_SIO_GPIO_OUT_SET     *(volatile uint32_t *)(SIO_BASE + 0x020)
#define PICO_SIO_GPIO_OE_XOR      *(volatile uint32_t *)(SIO_BASE + 0x01C)


int main()
{
    /*GPIO Driver config remark: 
    This access to set the GPIO as general IO mode: iobank0_hw->io[25].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB; 
    Has been replaced in a more readable line 22: GPIO will be acting as I/O mode */ 
    PICO_IOBANK_GPIO_CTRL = (5 << 0);

    /*2. Set as OUTPUT mode GPIO25*/
    PICO_SIO_GPIO_OUT_SET = (1 << 25);

    while (true) {

        /*Delay*/
        for(int i =0; i < 10000000; i++);
        PICO_SIO_GPIO_OE_XOR = (1 << 25); /*Toggle led*/
        
    }
}
