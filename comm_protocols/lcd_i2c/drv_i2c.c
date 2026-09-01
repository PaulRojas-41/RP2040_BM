#include "pico/stdlib.h"
#include "hardware/structs/sio.h"
#include "hardware/structs/iobank0.h"
#include "hardware/i2c.h"
#include "hardware/resets.h"
#include <stdint.h>
#include "header/drv_i2c.h"

//gpio 4 and 5
#define PICO_IOBANK_GPIO4_CTRL     *(volatile uint32_t *)(IO_BANK0_BASE + 0x024)
#define PICO_IOBANK_GPIO5_CTRL     *(volatile uint32_t *)(IO_BANK0_BASE + 0x02C)

// PADS base address map 
#define PICO_PADS_BANK0_BASE 0x4001c000

#define PICO_PADS_BANK0_GPIO4     *(volatile uint32_t *)(IO_BANK0_BASE + 0x14)
#define PICO_PADS_BANK0_GPIO5     *(volatile uint32_t *)(IO_BANK0_BASE + 0x18)

/* pointer to I2C0 peripheral address */
#define PICO_I2C0_BASE              (0x40044000u)
#define PICO_I2C0_CON              *(volatile uint32_t *)(PICO_I2C0_BASE + 0x00)
#define PICO_I2C0_TAR              *(volatile uint32_t *)(PICO_I2C0_BASE + 0x04)
#define PICO_I2C0_DATA_CMD         *(volatile uint32_t *)(PICO_I2C0_BASE + 0x10)
#define PICO_I2C0_IC_FS_SCL_HCNT   *(volatile uint32_t *)(PICO_I2C0_BASE + 0x1C)
#define PICO_I2C0_IC_FS_SCL_LCNT   *(volatile uint32_t *)(PICO_I2C0_BASE + 0x20)
#define PICO_I2C0_EN               *(volatile uint32_t *)(PICO_I2C0_BASE + 0x6C)
#define PICO_I2C0_STATUS           *(volatile uint32_t *)(PICO_I2C0_BASE + 0x70)
#define PICO_I2C0_RAW_INTR_STATUS  *(volatile uint32_t *)(PICO_I2C0_BASE + 0x34)

/*bit defs*/
#define PICO_I2C0_STOP_ISSUED_POS 9u
#define PICO_I2C0_TX_EMPTY        4u

void i2c0_reset_sys(void) 
{
    uint32_t i2c0_reset    = (1u << 3);
    uint32_t iobank0_reset = (1u << 5);
    uint32_t iopads_reset  = (1u << 8);

    resets_hw->reset = i2c0_reset;
    while(!(resets_hw->reset_done & i2c0_reset));

    resets_hw->reset = iobank0_reset;
    while(!(resets_hw->reset_done & iobank0_reset));

    resets_hw->reset = iopads_reset;
    while(!(resets_hw->reset_done & iopads_reset));
}

void i2c0_byte_tx(uint8_t *data, uint8_t length)
{
    uint8_t byte_cntr;
    uint8_t cmd;
    bool last_byte;

    PICO_I2C0_EN = 0x00;
    PICO_I2C0_TAR = 0x27; /*lcd address*/
    PICO_I2C0_EN = 0x01;

    /* bit 8 in zero, means a write op
       restart issued before sent byte */
    //PICO_I2C0_DATA_CMD &= ~(0 << 8);

    for(byte_cntr = 0; byte_cntr < length; byte_cntr++)
    {
        last_byte = ((length - byte_cntr) == 1 ? true : false);

        /*Sequence initiated by Restart for the first bit: enabled in ICON_Register */
        PICO_I2C0_DATA_CMD = ((1 << 10) | 
                             ((last_byte ? 1 : 0) << PICO_I2C0_STOP_ISSUED_POS) | 
                             (*data++)); 

        while(!(PICO_I2C0_RAW_INTR_STATUS & (1 << PICO_I2C0_TX_EMPTY)));
    }
}

void gpio_i2c0_init()
{
    // 1. Route GPIO 4 and 5 to the I2C0 peripheral function selector: 3
    PICO_IOBANK_GPIO4_CTRL = 3;
    PICO_IOBANK_GPIO5_CTRL = 3;

    PICO_PADS_BANK0_GPIO4 = 1 << 3;
    PICO_PADS_BANK0_GPIO5 = 1 << 3;
}

void i2c0_init_peripheral()
{
    //i2c0_reset_sys();

    PICO_I2C0_EN = 0x00;

    /*TX_EMPTY (1<<8) allows to control bus disponibility */
    PICO_I2C0_CON = I2C_IC_CON_SPEED_VALUE_FAST << I2C_IC_CON_SPEED_LSB |
                    (1 << 0) |
                    (1 << 6) |
                    (1 << 5) |
                    (1 << 8);   
    /*scl clk cntrs here:*/
    i2c0_scl_low_high_periods();

    PICO_I2C0_EN = 0x01;
}

void i2c0_scl_low_high_periods()
{
    /*baudrate: 400kbps= bits per second where
    the scl line toggles per bit transmitted on SDA line
    */
    PICO_I2C0_IC_FS_SCL_HCNT = 187;
    PICO_I2C0_IC_FS_SCL_LCNT = 125;

}