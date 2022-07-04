cmake_minimum_required(VERSION 3.21)
project(microRadar)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)

#set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(.)

if(UNIX AND APPLE)
	set(wxWidgets_ROOT_DIR /usr/local/Cellar/wxwidgets/3.1.5)
	include_directories(${wxWidgets_ROOT_DIR}/include)
endif()

if(NOT APPLE)
	set(wxWidgets_USE_STATIC 1)
endif()

find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

if(APPLE)
add_executable(microRadar
	#manifest.o
	main.cpp
	aboutDialog.cpp
	mainFrame.cpp
	persistenceManager.cpp
	phyConnectionManager.cpp
	filesystemLogManager.cpp
	settingsDialog.cpp
	connectionPanel.cpp
	measurePanel.cpp
	hw_detection.cpp
	logPanel.cpp
	gui_helpers.cpp
	welfordAvg.cpp
	triangularAvg.cpp
	umrrParser/umrrParser.cpp)
else()
	add_executable(microRadar
			manifest.o
			main.cpp
			aboutDialog.cpp
			mainFrame.cpp
			persistenceManager.cpp
			filesystemLogManager.cpp
			phyConnectionManager.cpp
            settingsDialog.cpp
			connectionPanel.cpp
			measurePanel.cpp
			hw_detection.cpp
			logPanel.cpp
			gui_helpers.cpp
			welfordAvg.cpp
			triangularAvg.cpp
			umrrParser/umrrParser.cpp)
endif()

target_link_libraries(microRadar -static)

if (NOT APPLE)
	target_link_libraries(microRadar -lmathplot)
endif()

target_link_libraries(microRadar ${wxWidgets_LIBRARIES})
