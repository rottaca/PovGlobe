cmake_minimum_required(VERSION 3.10)
project(PovGlobe)

option(BUILD_TARGET "Build simulation (SIM) or hw (HW)." "SIM")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CORE_LIB_NAME ${CMAKE_PROJECT_NAME}_core_lib)
set(HW_LIB_EXT_NAME ${CMAKE_PROJECT_NAME}_hw_lib_ext)
set(SIM_LIB_EXT_NAME ${CMAKE_PROJECT_NAME}_sim_lib_ext)
set(APPS_LIB_NAME ${CMAKE_PROJECT_NAME}_apps_lib)

set(EXEC_MAIN_NAME ${CMAKE_PROJECT_NAME})

# Create interface library for cimg header only lib
add_library(cimg INTERFACE)
target_include_directories(cimg INTERFACE ext/CImg-2.9.5_pre122220)

if (WIN32)
	message(STATUS "Building on windows.")
	if(${BUILD_TARGET} STREQUAL "HW")
		message(ERROR "Can't build hw software version on non-unix platform.")
	endif()

elseif(UNIX)
	message(STATUS "Building on linux.")
	set(CMAKE_CXX_FLAGS "-Wall -Wextra")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
else()
  message(FATAL_ERROR "Unknown platform.")
endif()


if (${BUILD_TARGET} STREQUAL "SIM")
	add_compile_definitions(BUILD_SIM)
	message(STATUS "Building simulation framework.")
elseif(${BUILD_TARGET} STREQUAL "HW")
	add_compile_definitions(BUILD_HW)
	message(STATUS "Building hw application.")
	add_subdirectory(ext/rpi_ws281x)
	add_subdirectory(ext/pigpio)
else()
  message(FATAL_ERROR "Invalid build target specified.")
endif()

add_subdirectory(src/core)

if (${BUILD_TARGET} STREQUAL "SIM")
  add_subdirectory(src/sim)
  set(ENV_LIB_TARGET_NAME ${SIM_LIB_EXT_NAME})
elseif(${BUILD_TARGET} STREQUAL "HW")
  add_subdirectory(src/hw)
  set(ENV_LIB_TARGET_NAME ${HW_LIB_EXT_NAME})
endif()


add_subdirectory(src/apps)

# executable
add_executable(${EXEC_MAIN_NAME}
	src/main.cpp
)
target_link_libraries(${EXEC_MAIN_NAME} ${APPS_LIB_NAME} ${ENV_LIB_TARGET_NAME} ${CORE_LIB_NAME})

# executable
add_executable(${EXEC_MAIN_NAME}_test_rpm
	src/main_test_rpm.cpp
)
target_link_libraries(${EXEC_MAIN_NAME}_test_rpm ${APPS_LIB_NAME} ${ENV_LIB_TARGET_NAME} ${CORE_LIB_NAME})
