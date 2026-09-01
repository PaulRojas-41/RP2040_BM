/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/structs/sio.h"
#include "hardware/structs/iobank0.h"
#include "hardware/clocks.h"
#include <stdint.h>
#include "header/drv_i2c.h"


/* Example code to drive a 16x2 LCD panel via a I2C bridge chip (e.g. PCF8574)

   NOTE: The panel must be capable of being driven at 3.3v NOT 5v. The Pico
   GPIO (and therefore I2C) cannot be used at 5v.

   You will need to use a level shifter on the I2C lines if you want to run the
   board at 5v.

   Connections on Raspberry Pi Pico board, other boards may vary.

   GPIO 4 (pin 6)-> SDA on LCD bridge board
   GPIO 5 (pin 7)-> SCL on LCD bridge board
   3.3v (pin 36) -> VCC on LCD bridge board
   GND (pin 38)  -> GND on LCD bridge board
*/
// commands
const int LCD_CLEARDISPLAY = 0x01;
const int LCD_RETURNHOME = 0x02;
const int LCD_ENTRYMODESET = 0x04;
const int LCD_DISPLAYCONTROL = 0x08;
const int LCD_CURSORSHIFT = 0x10;
const int LCD_FUNCTIONSET = 0x20;
const int LCD_SETCGRAMADDR = 0x40;
const int LCD_SETDDRAMADDR = 0x80;

// flags for display entry mode
const int LCD_ENTRYSHIFTINCREMENT = 0x01;
const int LCD_ENTRYLEFT = 0x02;

// flags for display and cursor control
const int LCD_BLINKON = 0x01;
const int LCD_CURSORON = 0x02;
const int LCD_DISPLAYON = 0x04;

// flags for display and cursor shift
const int LCD_MOVERIGHT = 0x04;
const int LCD_DISPLAYMOVE = 0x08;

// flags for function set
const int LCD_5x10DOTS = 0x04;
const int LCD_2LINE = 0x08;
const int LCD_8BITMODE = 0x10;

// flag for backlight control
const int LCD_BACKLIGHT = 0x08;

const int LCD_ENABLE_BIT = 0x04;

// By default these LCD display drivers are on bus address 0x27
static int addr = 0x27;

// Modes for lcd_send_byte
#define LCD_CHARACTER  1
#define LCD_COMMAND    0

#define MAX_LINES      2
#define MAX_CHARS      16

#define DELAY_US 600

void lcd_i2c0_write_byte(uint8_t val)
{
    i2c0_byte_tx(&val,1);
}

void lcd_toggle_enable(uint8_t val) 
{
    sleep_us(DELAY_US);
    lcd_i2c0_write_byte(val | LCD_ENABLE_BIT);
    sleep_us(DELAY_US);
    lcd_i2c0_write_byte(val & ~LCD_ENABLE_BIT);
    sleep_us(DELAY_US);
}

// The display is sent a byte as two separate nibble transfers
void lcd_send_byte(uint8_t val, int mode) 
{
    uint8_t high = mode | (val & 0xF0) | LCD_BACKLIGHT;
    uint8_t low = mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;

    lcd_i2c0_write_byte(high);
    lcd_toggle_enable(high);
    lcd_i2c0_write_byte(low);
    lcd_toggle_enable(low);
}

void lcd_clear(void) 
{
    lcd_send_byte(LCD_CLEARDISPLAY, LCD_COMMAND);
}

// go to location on LCD
void lcd_set_cursor(int line, int position) 
{
    int val = (line == 0) ? 0x80 + position : 0xC0 + position;
    lcd_send_byte(val, LCD_COMMAND);
}

static inline void lcd_char(char val) 
{
    lcd_send_byte(val, LCD_CHARACTER);
}

void lcd_string(const char *s) 
{
    while (*s) {
        lcd_char(*s++);
    }
}

void lcd_init() 
{
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x02, LCD_COMMAND);

    lcd_send_byte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    lcd_send_byte(LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND);
    lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);

    lcd_clear();
}

int main() 
{
    i2c0_init_peripheral();
    gpio_i2c0_init();

    lcd_init();

    static char *message[] = {"Hola", "RP2040", "Cortex M0","Bare Metal",
                            "Design", "I2C0 BUS", "NO DEBUGGER",
                            "LCD", "VSCODE", "CMAKE","NO SDK"};

    while (1) 
    {
        for (uint m = 0; m < sizeof(message) / sizeof(message[0]); m += MAX_LINES) 
        {
            for (int line = 0; line < MAX_LINES; line++) 
            {
                lcd_set_cursor(line, (MAX_CHARS / 2) - strlen(message[m + line]) / 2);
                lcd_string(message[m + line]);
            }
            sleep_ms(2000);
            lcd_clear();
        }
    }
}
