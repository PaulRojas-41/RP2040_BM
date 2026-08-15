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

add_subdirectory(gpio)