# Change gpio_driver to gpio_led_blink
add_executable(led
    led.c
)

# Pull in common dependencies
target_link_libraries(led pico_stdlib)

# Create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(led)