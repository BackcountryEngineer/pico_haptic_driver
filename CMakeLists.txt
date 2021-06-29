cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico_haptic_controls C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_HAPTIC_CONTROLS_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

link_directories(pico_uros_sdk/libmicroros)

add_executable(i2c_bus_scan
    bus_scan.c
)

target_link_libraries(i2c_bus_scan 
    pico_stdlib 
    hardware_i2c
)

add_executable(pico_haptic_control
    pico_haptic_control.c
    pico_uros_sdk/pico_uart_transport.c
    drv2605.c
)

target_link_libraries(pico_haptic_control 
    pico_stdlib 
    hardware_i2c
    microros
)

target_include_directories(pico_haptic_control
    PUBLIC
    pico_uros_sdk/libmicroros/include
)

SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")

add_compile_options(-Wall
    -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
    -Wno-unused-function # we have some for the docs that aren't called
    -Wno-maybe-uninitialized
)

add_compile_definitions(PICO_UART_ENABLE_CRLF_SUPPORT=0)
add_compile_definitions(PICO_STDIO_ENABLE_CRLF_SUPPORT=0)
add_compile_definitions(PICO_STDIO_DEFAULT_CRLF=0)