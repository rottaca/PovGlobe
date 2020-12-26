cmake_minimum_required(VERSION 3.10)
project(PovGlobe)

if (UNIX)
	add_subdirectory(ext/rpi_ws281x-master)
endif()

set(SOURCE_FILES
	src/globe.cpp
	src/application_base.cpp
	src/rpm_measure_base.cpp
	src/renderer_base.cpp
	src/helper.cpp)

set(PUBLIC_HEADER 
	src/globe.hpp
	src/rpm_measure_base.hpp
	src/application_base.hpp
	src/renderer_base.hpp
	src/helper.hpp
)

if (WIN32)
	message(STATUS "Building sim framework for windows.")
	list(APPEND SOURCE_FILES 
				src/rpm_measure_sim.cpp 
				src/renderer_sim.cpp)
else()
	message(STATUS "Building framework for linux.")
	list(APPEND SOURCE_FILES 
				src/rpm_measure_hall.cpp 
				src/renderer_led_strip.cpp)
endif()

add_library(${CMAKE_PROJECT_NAME}_lib
	${SOURCE_FILES}
)
set_target_properties(${CMAKE_PROJECT_NAME}_lib PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADER}")


if (WIN32)
	target_link_libraries(${CMAKE_PROJECT_NAME}_lib)
elseif (UNIX)
	target_link_libraries(${CMAKE_PROJECT_NAME}_lib ws2811)
endif()



add_executable(${CMAKE_PROJECT_NAME}
	src/main.cpp
)
target_link_libraries(${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}_lib)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ext/CImg-2.9.5_pre122220)