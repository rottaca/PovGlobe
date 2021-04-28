cmake_minimum_required(VERSION 3.10)
project(PovGlobe)

set(EXT_DIR ./ext)

set(CORE_LIB_NAME ${CMAKE_PROJECT_NAME}_core_lib)
set(HW_LIB_EXT_NAME ${CMAKE_PROJECT_NAME}_hw_lib_ext)
set(SIM_LIB_EXT_NAME ${CMAKE_PROJECT_NAME}_sim_lib_ext)
set(APPS_LIB_NAME ${CMAKE_PROJECT_NAME}_apps_lib)

set(PY_WRAPPER_LIB_NAME Py${CMAKE_PROJECT_NAME})

set(EXEC_MAIN_NAME ${CMAKE_PROJECT_NAME})

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create interface library for cimg header only lib
add_library(cimg INTERFACE)
target_include_directories(cimg INTERFACE ${EXT_DIR}/CImg-2.9.5_pre122220)

if (WIN32)
	message(STATUS "Building on windows.")
elseif(UNIX)
	message(STATUS "Building on linux.")
	set(CMAKE_CXX_FLAGS "-Wall -Wextra")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
else()
  message(FATAL_ERROR "Unknown platform.")
endif()

add_subdirectory(cpp_src)

if(UNIX)
include(ExternalProject)
ExternalProject_Add(pico_firmware
  SOURCE_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/pico_src/Renderer"
  BINARY_DIR        "${CMAKE_BINARY_DIR}/pico_firmware"
  INSTALL_COMMAND cmake -E echo "Skipping install step."
)
endif()