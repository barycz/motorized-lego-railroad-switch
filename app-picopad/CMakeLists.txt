cmake_minimum_required(VERSION 3.13)

set(FIRMWARE app-picopad)

add_subdirectory(external/hagl)
add_subdirectory(external/hagl_hal)

file(GLOB_RECURSE FIRMWARE_SOURCES "src/*.cpp" "src/*.c" "../shared/*.cpp" "../shared/*.c")

add_executable(${FIRMWARE} ${FIRMWARE_SOURCES})

pico_enable_stdio_usb(${FIRMWARE} 1)
pico_enable_stdio_uart(${FIRMWARE} 0)

pico_add_extra_outputs(${FIRMWARE})

target_compile_definitions(${FIRMWARE} PRIVATE
	WIFI_SSID=\"${WIFI_SSID}\"
	WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
)

target_include_directories(${FIRMWARE} PRIVATE
	src
	../shared
	external/embedded-fonts
)

target_link_libraries(${FIRMWARE}
	pico_cyw43_arch_lwip_poll
	pico_stdlib
	hagl
	hagl_hal
)

target_compile_definitions(${FIRMWARE} PRIVATE
	HAGL_HAL_USE_DOUBLE_BUFFER
	HAGL_CHAR_BUFFER_SIZE=208
)

include(boards/Picopad.cmake)
