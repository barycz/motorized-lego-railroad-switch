cmake_minimum_required(VERSION 3.12)

set(PICO_BOARD pico_w)

include(pico_sdk_import.cmake)

project(train-switch-firmware C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(train-switch-firmware src/main.cpp)

pico_enable_stdio_usb(train-switch-firmware 1)
pico_enable_stdio_uart(train-switch-firmware 0)

pico_add_extra_outputs(train-switch-firmware)

target_include_directories(train-switch-firmware PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries(train-switch-firmware
	pico_cyw43_arch_lwip_poll
	pico_stdlib
	hardware_pwm
)
