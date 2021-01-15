cmake_minimum_required(VERSION 3.10)
project(PovGlobe)


set(CORE_LIB_NAME ${CMAKE_PROJECT_NAME}_core_lib)
set(HW_LIB_EXT_NAME ${CMAKE_PROJECT_NAME}_hw_lib_ext)
set(SIM_LIB_EXT_NAME ${CMAKE_PROJECT_NAME}_sim_lib_ext)
set(APPS_LIB_NAME ${CMAKE_PROJECT_NAME}_apps_lib)

set(PY_WRAPPER_LIB_NAME ${CMAKE_PROJECT_NAME}_py_wrapper)

set(EXEC_MAIN_NAME ${CMAKE_PROJECT_NAME})


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
  add_subdirectory(ext/pigpio)
else()
  message(FATAL_ERROR "Unknown platform.")
endif()

add_subdirectory(src/core)
add_subdirectory(src/apps)

set(POV_GLOBE_TARGETS "")
list(APPEND POV_GLOBE_TARGETS ${CORE_LIB_NAME})
list(APPEND POV_GLOBE_TARGETS ${APPS_LIB_NAME})

add_subdirectory(src/hw)
if(TARGET ${HW_LIB_EXT_NAME})
  list(APPEND POV_GLOBE_TARGETS ${HW_LIB_EXT_NAME})
endif()

add_subdirectory(src/sim)
if(TARGET ${SIM_LIB_EXT_NAME})
  list(APPEND POV_GLOBE_TARGETS ${SIM_LIB_EXT_NAME})
endif()

add_subdirectory(src/wrapper_python)

# executable
add_executable(${EXEC_MAIN_NAME}
	src/main.cpp
)
target_link_libraries(${EXEC_MAIN_NAME} ${POV_GLOBE_TARGETS})

# executable
add_executable(${EXEC_MAIN_NAME}_test_rpm
	src/main_test_rpm.cpp
)
target_link_libraries(${EXEC_MAIN_NAME}_test_rpm ${POV_GLOBE_TARGETS})
