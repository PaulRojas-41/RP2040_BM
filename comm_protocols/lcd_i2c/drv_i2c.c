#include "pico/stdlib.h"
#include "hardware/structs/sio.h"
#include "hardware/structs/iobank0.h"
#include "hardware/i2c.h"
#include "hardware/resets.h"
#include <stdint.h>
#include "header/drv_i2c.h"

//led
#define PICO_IO_BANK0_BASE 0x40014000
#define PICO_IOBANK_GPIO_CTRL     *(volatile uint32_t *)(IO_BANK0_BASE + 0x0cc) //GPIO25 CTRL reg (0x400140cc)
#define PICO_SIO_BASE      0xd0000000
#define PICO_SIO_GPIO_OUT_SET     *(volatile uint32_t *)(SIO_BASE + 0x020)

#define PICO_IO_BANK0_BASE 0x40014000
#define PICO_IOBANK_GPIO25_CTRL     *(volatile uint32_t *)(PICO_IO_BANK0_BASE + 0x0cc) //GPIO25 CTRL reg (0x400140cc)

//gpio 4 and 5
#define PICO_IOBANK_GPIO4_CTRL     *(volatile uint32_t *)(IO_BANK0_BASE + 0x024)
#define PICO_IOBANK_GPIO5_CTRL     *(volatile uint32_t *)(IO_BANK0_BASE + 0x02C)

//gpio 18 and 19
#define PICO_IOBANK_GPIO18_CTRL     *(volatile uint32_t *)(IO_BANK0_BASE + 0x094)
#define PICO_IOBANK_GPIO19_CTRL     *(volatile uint32_t *)(IO_BANK0_BASE + 0x09C)

// PADS base address map 
#define PICO_PADS_BANK0_BASE 0x4001c000

#define PICO_PADS_BANK0_GPIO4     *(volatile uint32_t *)(IO_BANK0_BASE + 0x14)
#define PICO_PADS_BANK0_GPIO5     *(volatile uint32_t *)(IO_BANK0_BASE + 0x18)

#define PICO_PADS_BANK0_GPIO2     *(volatile uint32_t *)(IO_BANK0_BASE + 0x0C)
#define PICO_PADS_BANK0_GPIO3    *(volatile uint32_t *)(IO_BANK0_BASE + 0x10)

#define PICO_SIO_BASE      0xd0000000
#define PICO_SIO_GPIO_OUT_SET     *(volatile uint32_t *)(SIO_BASE + 0x020)
#define PICO_SIO_GPIO_OE_XOR      *(volatile uint32_t *)(SIO_BASE + 0x01C)

// Base address map pointers for XOSC
#define PICO_XOSC_BASE     (0x40024000u)
#define PICO_XOSC          ((XOSC_Type *)XOSC_BASE)
#define PICO_XOSC_SET      ((XOSC_Type *)(XOSC_BASE + 0x1000)) // Atomic bit set alias
#define PICO_XOSC_CLR      ((XOSC_Type *)(XOSC_BASE + 0x2000)) // Atomic bit clear alias

/* pointer to I2C0 peripheral address */
#define PICO_I2C0_BASE        (0x40044000u)
#define PICO_I2C0_CON         *(volatile uint32_t *)(PICO_I2C0_BASE + 0x00)
#define PICO_I2C0_TAR            *(volatile uint32_t *)(PICO_I2C0_BASE + 0x04)
#define PICO_I2C0_DATA_CMD       *(volatile uint32_t *)(PICO_I2C0_BASE + 0x10)
#define PICO_I2C0_IC_FS_SCL_HCNT *(volatile uint32_t *)(PICO_I2C0_BASE + 0x1C)
#define PICO_I2C0_IC_FS_SCL_LCNT *(volatile uint32_t *)(PICO_I2C0_BASE + 0x20)
#define PICO_I2C0_EN             *(volatile uint32_t *)(PICO_I2C0_BASE + 0x6C)
#define PICO_I2C0_STATUS         *(volatile uint32_t *)(PICO_I2C0_BASE + 0x70)


typedef struct {
    volatile uint32_t ctrl;    // 0x00: Crystal Oscillator Control
    volatile uint32_t status;  // 0x04: Crystal Oscillator Status
    volatile uint32_t startup; // 0x08: Crystal Oscillator Startup Delay
    volatile uint32_t count;   // 0x0C: Crystal Oscillator Count Down
} XOSC_Type;

#define XOSC_BASE           (0x40024000u)
#define XOSC                ((XOSC_Type *)XOSC_BASE)
#define XOSC_SET            ((XOSC_Type *)(XOSC_BASE + 0x1000)) // Atomic bit set

typedef struct {
    volatile uint32_t ctrl;     // Control Register
    volatile uint32_t div;      // Divider Register
    volatile uint32_t selected; // Select Status Register
} CLK_CHAN_Type;

typedef struct {
    CLK_CHAN_Type clk_ref;      // 0x00: Reference Clock Channel
    CLK_CHAN_Type clk_sys;      // 0x0C: System Clock Channel
    CLK_CHAN_Type clk_peri;     // 0x18: Peripheral Clock Channel
} CLOCKS_Type;

#define CLOCKS_BASE         (0x40008000u)
#define CLOCKS              ((CLOCKS_Type *)CLOCKS_BASE)

void i2c0_reset_sys(void) 
{
    uint32_t i2c0_reset   = (1u << 3);
    uint32_t iobank_reset = (1u << 6);
    uint32_t iopads_reset = (1u << 9);

    reset_block_mask((i2c0_reset) | (iobank_reset) | (iopads_reset));
    unreset_block_mask_wait_blocking((i2c0_reset) | (iobank_reset) | (iopads_reset));

}

void setupClocks(void)
{
    // 1. Configure the XOSC frequency profile for a standard 1-15MHz crystal
    XOSC->ctrl = 0xAA0; // Magic bit pattern for standard 12MHz operation profile
    
    // 2. Set the startup time delay constraint.
    // Gives the crystal enough cycles to stabilize before the chip samples it.
    XOSC->startup = 0xC4; 

    // 3. Enable the XOSC using the atomic bit-set register
    XOSC_SET->ctrl = 0xFAB000; // 0xFAB is the enable magic word token prefix

    // 4. Poll the hardware STATUS register until the STABLE bit (Bit 12) flips to 1
    while ((XOSC->status & (1u << 12)) == 0);

    // 5. Route the Reference Clock (clk_ref) source directly to XOSC (Source index = 2)
    CLOCKS->clk_ref.ctrl = 2;

    // 6. Route the System Clock (clk_sys) source directly to clk_ref (Source index = 0)
    // This shifts the whole execution pipeline to run cleanly off the crystal.
    CLOCKS->clk_sys.ctrl = 0;

    // 7. Route the Peripheral Clock (clk_peri) source to watch the XOSC auxiliary path
    // AUXSRC field is bits [7:5]. Option 4 connects it directly to XOSC.
    CLOCKS->clk_peri.ctrl = (4u << 5);

    // 8. Turn on the global clk_peri enable bit (Bit 11) to feed clock lines to I2C/SPI
    CLOCKS->clk_peri.ctrl |= (1u << 11);
}

void test_i2c0()
{
    stdio_init_all();

    uint8_t i;
    uint8_t data = 0xAA;

    PICO_I2C0_EN = 0x00;
    /* slave's address */
    PICO_I2C0_TAR = 0x27;
    PICO_I2C0_EN = 0x01;
    PICO_I2C0_DATA_CMD = (0 << 8) | (1 << 9) | 0xAA; // wr op | stop bit   
    
    while(PICO_I2C0_STATUS & 0x01);

    for(int i = 0; i < 10000000; i++);
}

void i2c0_byte_tx(uint8_t *data, uint8_t length)
{
    uint8_t i;

    PICO_I2C0_EN = 0x00;

    /* slave's address */
    PICO_I2C0_TAR = 0x27;

    PICO_I2C0_EN = 0x01;

    if(length == 0) return;

    /* bit 8 in zero, means a write op
       restart issued before sent byte */
    PICO_I2C0_DATA_CMD &= ~(0 << 8);
    PICO_I2C0_DATA_CMD |= (1 << 10);

    for(i = 0; i < length; i++)
    {
        /*we check if tx fifo is empty */
         if(!(PICO_I2C0_STATUS & (1 << 1)))
         {
             /* packet format:  */
             uint8_t cmd = (data[i] & 0xFF);

             if(i == 0)
             {
                /* Restart condition issued */
                 cmd|= (1 << 10);
             }

             if(i == length - 1)
             {
                /* Stop condition issued */
                 cmd|= (1 << 9);
             }

             PICO_I2C0_DATA_CMD = cmd; 
         }
    }

}

void gpio_i2c0_init()
{
    // LED 
    PICO_IOBANK_GPIO_CTRL = (5 << 0);
    /*2. Set as OUTPUT mode GPIO25*/
    PICO_SIO_GPIO_OUT_SET = (1 << 25);

    // 1. Route GPIO 4 and 5 to the I2C0 peripheral function selector: 3, datasheet 
    PICO_IOBANK_GPIO4_CTRL = 3;
    PICO_IOBANK_GPIO5_CTRL = 3;

    PICO_PADS_BANK0_GPIO4 = 1 << 3;
    PICO_PADS_BANK0_GPIO5 = 1 << 3;

    // 1. Route GPIO 2 (SDA) and 3(scl) to the I2C1 peripheral function selector: 4, datasheet 

}

void i2c0_init_peripheral()
{
    i2c0_reset_sys();

    PICO_I2C0_CON &=~(1 << 5); // disable restart 
    PICO_I2C0_EN = 0x00;

    PICO_I2C0_CON = I2C_IC_CON_SPEED_VALUE_FAST << I2C_IC_CON_SPEED_LSB |
                    (1 << 0) |
                    (1 << 6) |
                    (1 << 5) |
                    I2C_IC_CON_TX_EMPTY_CTRL_BITS;
    /* slave's address */
    PICO_I2C0_TAR = 0x27;
    
    /*scl clk cntrs here:*/
    i2c0_scl_low_high_periods();

    PICO_I2C0_EN = 0x01;
}

void i2c0_scl_low_high_periods()
{
    /*taking 400kbps = 1 / 400 kbps = 2.5us 
    (12 Mhz / 400 kbps ) / 2 = 30 cycles 
    16 cycles for low_phase and 14 for high_phase: datasheet table 450, pg463 formulas */
    PICO_I2C0_EN = 0x00;
    PICO_I2C0_IC_FS_SCL_HCNT = 6;
    PICO_I2C0_IC_FS_SCL_LCNT = 15;
    PICO_I2C0_EN = 0x01;
}