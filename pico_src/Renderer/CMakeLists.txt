# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Renderer C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(Renderer main.cpp RTTMeasure.cpp UartDataReader.cpp LEDController.cpp)

pico_generate_pio_header(Renderer ${CMAKE_CURRENT_LIST_DIR}/apa102.pio)

pico_set_program_name(Renderer "Renderer")
pico_set_program_version(Renderer "0.1")

pico_enable_stdio_usb(Renderer 1)
pico_enable_stdio_uart(Renderer 0)

# Add the standard library to the build
target_link_libraries(Renderer pico_stdlib)

# Add any user requested libraries
target_link_libraries(Renderer
        hardware_interp
        hardware_timer
        hardware_clocks
        hardware_pio
        pico_multicore
        pico_sync
        pico_time
        pico_mem_ops
)

pico_add_extra_outputs(Renderer)

