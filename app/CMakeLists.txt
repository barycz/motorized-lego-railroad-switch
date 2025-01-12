cmake_minimum_required(VERSION 3.16) # Or a suitable minimum version

project(RailroadSwitchAppProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17) # Or your preferred C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Gui Network Widgets REQUIRED) # Find Qt6 modules

qt_standard_project_setup()

# Define source files
set(SOURCES
	src/Device.h
	src/DeviceManager.cpp
	src/DeviceManager.h
	src/DeviceWidget.cpp
	src/DeviceWidget.h
	src/Main.cpp
	src/MainWindow.cpp
	src/MainWindow.h
)

set(FORMS
	src/DeviceWidget.ui
	src/MainWindow.ui
)

set(RESOURCES
	src/resources.qrc
)

# Create executable
qt_add_executable(RailroadSwitchApp ${SOURCES} ${FORMS} ${RESOURCES})

# Link with Qt modules
target_link_libraries(RailroadSwitchApp PRIVATE Qt6::Core Qt6::Gui Qt6::Network Qt6::Widgets)

# Set target properties (optional)
set_target_properties(RailroadSwitchApp PROPERTIES
	CXX_STANDARD 17 # Ensure consistency
	CXX_STANDARD_REQUIRED ON
)

# Install target (optional)
install(TARGETS RailroadSwitchApp DESTINATION bin)
