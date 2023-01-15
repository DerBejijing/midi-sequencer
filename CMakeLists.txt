# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/Users/derbejijing/Documents/coding/raspberrypico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(midi-sequencer C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(midi-sequencer
        src/main.c
        src/seq_gpio.c
        src/seq_interface.c
        src/seq_sequencer.c
        src/seq_seven_segment.c
        )

pico_set_program_name(midi-sequencer "midi-sequencer")
pico_set_program_version(midi-sequencer "0.1")

pico_enable_stdio_uart(midi-sequencer 0)
pico_enable_stdio_usb(midi-sequencer 1)

# Add the standard library to the build
target_link_libraries(midi-sequencer pico_stdlib)

# Add any user requested libraries
target_link_libraries(midi-sequencer
        hardware_timer
        hardware_adc
        )

pico_add_extra_outputs(midi-sequencer)

