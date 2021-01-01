cmake_minimum_required(VERSION 3.10)
project(PovGlobe)

option(BUILD_TARGET "Build simulation (SIM) or hw (HW)." "SIM")

set(SOURCE_FILES
	"src/core/globe.cpp"
	"src/core/application_base.cpp"
	"src/core/rpm_measure_base.cpp"
	"src/core/renderer_base.cpp"
	"src/core/helper.cpp"
	"src/apps/application_examples.cpp")

set(SOURCE_FILES_SIM 
	src/sim/rpm_measure_sim.cpp  
	src/sim/renderer_sim.cpp)

set(SOURCE_FILES_HW
	src/hw/rpm_measure_hall.cpp 
	src/hw/renderer_led_strip.cpp)
	

if (WIN32)
	message(STATUS "Building on windows.")
else()
	message(STATUS "Building on linux.")
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (${BUILD_TARGET} STREQUAL "SIM")
	list(APPEND SOURCE_FILES ${SOURCE_FILES_SIM})
	add_compile_definitions(BUILD_SIM)
	message(STATUS "Building simulation framework.")

elseif(${BUILD_TARGET} STREQUAL "HW")
	if (NOT UNIX)
		message(ERROR "Can't build hw software VERSION on non-unix platform.")
	else()
		list(APPEND SOURCE_FILES ${SOURCE_FILES_HW})
		add_subdirectory(ext/rpi_ws281x)
		add_subdirectory(ext/pigpio)
		message(STATUS "Building hw application.")
		add_compile_definitions(BUILD_HW)
	endif()
endif()

# Build library
add_library(${CMAKE_PROJECT_NAME}_lib
	${SOURCE_FILES}
)
target_include_directories(${CMAKE_PROJECT_NAME}_lib PRIVATE ext/CImg-2.9.5_pre122220)

find_package(ImageMagick REQUIRED COMPONENTS Magick++)
target_link_libraries(${CMAKE_PROJECT_NAME}_lib ${ImageMagick_LIBRARIES})
target_include_directories(${CMAKE_PROJECT_NAME}_lib PUBLIC ext/CImg-2.9.5_pre122220 ${ImageMagick_INCLUDE_DIRS})
add_compile_definitions(cimg_use_magick)


if (${BUILD_TARGET} STREQUAL "SIM")

  if (WIN32)
    target_link_libraries(${CMAKE_PROJECT_NAME}_lib gdi32)
  elseif (UNIX)
    find_package(X11 REQUIRED)
    target_link_libraries(${CMAKE_PROJECT_NAME}_lib pthread ${X11_LIBRARIES})
  endif()
  target_include_directories(${CMAKE_PROJECT_NAME}_lib PUBLIC src)
elseif(${BUILD_TARGET} STREQUAL "HW")
  target_link_libraries(${CMAKE_PROJECT_NAME}_lib pthread ws2811 pigpio)
  target_include_directories(${CMAKE_PROJECT_NAME}_lib PUBLIC ext/pigpio ext/rpi_ws281x src)
endif()

# Build executable
add_executable(${CMAKE_PROJECT_NAME}
	src/main.cpp
)
target_link_libraries(${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}_lib)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ext/CImg-2.9.5_pre122220)

# Build executable
add_executable(${CMAKE_PROJECT_NAME}_test_rpm
	src/main_test_rpm.cpp
)
target_link_libraries(${CMAKE_PROJECT_NAME}_test_rpm ${CMAKE_PROJECT_NAME}_lib)
