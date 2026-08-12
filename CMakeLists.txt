# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.0)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# 1. Set minimum CMake version required by Pico SDK
cmake_minimum_required(VERSION 3.13)

# 2. Include the SDK import script (MUST occur before project())
include(pico_sdk_import.cmake)

# 3. Define project details and languages (C, C++, Assembly)
project(RP2040_BM C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# 4. Initialize the Pico SDK environment for the libraries
pico_sdk_init()

# 5. Add your executable target
add_executable(RP2040_BM
    main.c
)

# 6. Link required libraries from the Pico SDK
target_link_libraries(RP2040_BM
    pico_stdlib         # Core hardware APIs (GPIO, UART, Sleep, etc.)
    # hardware_adc      # Uncomment to use ADC hardware library
    # hardware_i2c      # Uncomment to use I2C hardware library
    # hardware_spi      # Uncomment to use SPI hardware library
    # hardware_pwm      # Uncomment to use PWM hardware library
)

# 7. Create map, bin, hex, and .uf2 output targets
pico_add_extra_outputs(RP2040_BM)