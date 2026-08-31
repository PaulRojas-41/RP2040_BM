/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/structs/sio.h"
#include "hardware/structs/iobank0.h"
#include "hardware/clocks.h"
#include <stdint.h>
#include "header/drv_i2c.h"

int main(void) 
{
    stdio_init_all();
    setupClocks();
    i2c0_init_peripheral();
    gpio_i2c0_init();

    //lcd_init(&my_display, i2c0_byte_tx, 0x27);

    while (1) 
    {
        test_i2c0();
        for(int i = 0; i < 10000000; i++);
    }
}