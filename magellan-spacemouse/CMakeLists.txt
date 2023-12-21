cmake_minimum_required(VERSION 3.13)

# add_compile_definitions(PICO_DEFAULT_UART_BAUD_RATE=921600)
add_compile_definitions(PICO_DEFAULT_UART_BAUD_RATE=115200)

set(PICO_SDK_PATH "C:/Program Files/Raspberry Pi/Pico SDK v1.5.1/pico-sdk")
set(PICO_TINYUSB_PATH "C:/Program Files/Raspberry Pi/Pico SDK v1.5.1/pico-sdk/lib/tinyusb")

include(pico_sdk_import.cmake)

project(magellan)

pico_sdk_init()

add_compile_options(-Wall)

add_executable(magellan
    src/magellan.c
    src/descriptors.c
)

target_include_directories(magellan PRIVATE src)

target_link_libraries(magellan
    pico_stdlib
    tinyusb_device
    tinyusb_board
)

pico_add_extra_outputs(magellan)
