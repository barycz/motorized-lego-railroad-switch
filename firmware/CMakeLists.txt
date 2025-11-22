cmake_minimum_required(VERSION 3.12)

set(PICO_BOARD pico_w)

if (DEFINED ENV{WIFI_SSID} AND (NOT WIFI_SSID))
	set(WIFI_SSID $ENV{WIFI_SSID})
	message("Using WIFI_SSID from environment ('${WIFI_SSID}')")
endif()

if (DEFINED ENV{WIFI_PASSWORD} AND (NOT WIFI_PASSWORD))
	set(WIFI_PASSWORD $ENV{WIFI_PASSWORD})
	message("Using WIFI_PASSWORD from environment")
endif()

set(WIFI_SSID "${WIFI_SSID}" CACHE INTERNAL "WiFi SSID for the firmware")
set(WIFI_PASSWORD "${WIFI_PASSWORD}" CACHE INTERNAL "WiFi password for the firmware")

if ("${WIFI_SSID}" STREQUAL "")
	message(WARNING "Will not work properly as WIFI_SSID is not defined")
elseif ("${WIFI_PASSWORD}" STREQUAL "")
	message(WARNING "Will not work properly as WIFI_PASSWORD is not defined")
endif()

include(pico_sdk_import.cmake)

project(train-switch-firmware C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(train-switch-firmware
	src/main.cpp
	../shared/RailwayProtocol.h
	../shared/RailwayProtocol.cpp
)

pico_enable_stdio_usb(train-switch-firmware 1)
pico_enable_stdio_uart(train-switch-firmware 0)

pico_add_extra_outputs(train-switch-firmware)

target_compile_definitions(train-switch-firmware PRIVATE
	WIFI_SSID=\"${WIFI_SSID}\"
	WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
)

target_include_directories(train-switch-firmware PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/src
	${CMAKE_CURRENT_LIST_DIR}/../shared
)

target_link_libraries(train-switch-firmware
	pico_cyw43_arch_lwip_poll
	pico_stdlib
	hardware_pwm
)
