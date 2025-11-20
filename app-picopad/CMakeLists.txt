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

set(FIRMWARE app-picopad)

project(${FIRMWARE} C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_subdirectory(external/hagl)
add_subdirectory(external/hagl_hal)

add_executable(${FIRMWARE}
	src/main.cpp
)

# the simplest way to get running on Picopad using the provided SDK and an SD card
pico_set_binary_type(${FIRMWARE} no_flash)

pico_enable_stdio_usb(${FIRMWARE} 1)
pico_enable_stdio_uart(${FIRMWARE} 0)

pico_add_extra_outputs(${FIRMWARE})

target_compile_definitions(${FIRMWARE} PRIVATE
	WIFI_SSID=\"${WIFI_SSID}\"
	WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
)

target_include_directories(${FIRMWARE} PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries(${FIRMWARE}
	pico_cyw43_arch_lwip_poll
	pico_stdlib
	hagl
	hagl_hal
)

target_compile_definitions(${FIRMWARE} PRIVATE
	HAGL_HAL_USE_DOUBLE_BUFFER
	HAGL_HAL_USE_DMA
	HAGL_HAL_DEBUG
)

include(boards/Picopad.cmake)
