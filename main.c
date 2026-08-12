/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "hardware/structs/sio.h"
#include "hardware/structs/iobank0.h"

int main()
{
    //Set pin 25 function to SIO (Software Control)
    iobank0_hw->io[25].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB; //     |0000| |0101|
    
    // Initialize the onboard LED GPIO pin (Pin 25 on RP2040 Pico)

    sio_hw->gpio_oe_set = (1u << 25);

    while (true) {

        for(int i =0; i < 100000000; i++); //delay();
        sio_hw->gpio_togl = (1 << 25);
        
    }
}
