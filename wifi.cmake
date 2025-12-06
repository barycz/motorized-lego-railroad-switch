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
