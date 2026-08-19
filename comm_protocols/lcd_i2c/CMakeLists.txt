# Change gpio_driver to gpio_led_blink
add_executable(lcd_i2c
    lcd_i2c.c
)

# Pull in common dependencies
target_link_libraries(lcd_i2c pico_stdlib hardware_i2c)

# Create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(lcd_i2c)