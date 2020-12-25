cmake_minimum_required(VERSION 3.10)
project(PovGlobe)

if (UNIX)
	add_subdirectory(ext/rpi_ws281x-master)
endif()

set(SOURCE_FILES
	src/main.cpp
	src/globe.cpp
)

if (WIN32)
	list(APPEND SOURCE_FILES src/rpm_measure_sim.cpp)
	list(APPEND SOURCE_FILES src/renderer_sim.cpp)
else()
	list(APPEND SOURCE_FILES src/rpm_measure_hall.cpp)
	list(APPEND SOURCE_FILES src/renderer_hall.cpp)
endif()

add_executable(${CMAKE_PROJECT_NAME}
	${SOURCE_FILES}
)
target_link_libraries(${CMAKE_PROJECT_NAME})