#ifndef _DRV_I2C_H
#define _DRV_I2C_H

#include <stdint.h>
#include "pico/stdlib.h"

void i2c0_init_peripheral();
void i2c0_scl_low_high_periods();
void gpio_i2c0_init();
void i2c0_byte_tx(uint8_t *data, uint8_t length);
void i2c0_reset_sys(void);
void setupClocks(void);
void test_i2c0(void);

#endif