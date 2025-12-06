cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE FIRMWARE_SOURCES "src/*.cpp" "src/*.c" "../shared/*.cpp" "../shared/*.c")

add_executable(train-switch-firmware ${FIRMWARE_SOURCES})

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
