cmake_minimum_required(VERSION 3.10)
project(PovGlobe)

option(BUILD_TARGET "Build simulation (SIM) or hw (HW)." "SIM")

set(SOURCE_FILES
	src/globe.cpp
	src/application_base.cpp
	src/rpm_measure_base.cpp
	src/renderer_base.cpp
	src/helper.cpp)

set(SOURCE_FILES_SIM 
	src/rpm_measure_sim.cpp  
	src/renderer_sim.cpp)

set(SOURCE_FILES_HW
	src/rpm_measure_hall.cpp 
	src/renderer_led_strip.cpp)
	

if (WIN32)
	message(STATUS "Building on windows.")
else()
	message(STATUS "Building on linux.")
endif()


if (${BUILD_TARGET} STREQUAL "SIM")
	list(APPEND SOURCE_FILES ${SOURCE_FILES_SIM})
	add_compile_definitions(BUILD_SIM)
	message(STATUS "Building simulation framework.")
elseif(${BUILD_TARGET} STREQUAL "HW")
	if (NOT UNIX)
		message(ERROR "Can't build hw software VERSION on non-unix platform.")
	else()
		list(APPEND SOURCE_FILES ${SOURCE_FILES_HW})
		add_compile_definitions(BUILD_HW)
		message(STATUS "Building hw application.")
	endif()
endif()

# Build library
add_library(${CMAKE_PROJECT_NAME}_lib
	${SOURCE_FILES}
)
target_include_directories(${CMAKE_PROJECT_NAME}_lib PRIVATE ext/CImg-2.9.5_pre122220)


if (WIN32)
	#target_link_libraries(${CMAKE_PROJECT_NAME}_lib )
elseif (UNIX)
	add_subdirectory(ext/rpi_ws281x)
	target_link_libraries(${CMAKE_PROJECT_NAME}_lib ws2811)
endif()


# Build executable
add_executable(${CMAKE_PROJECT_NAME}
	src/main.cpp
)
target_link_libraries(${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}_lib)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ext/CImg-2.9.5_pre122220)