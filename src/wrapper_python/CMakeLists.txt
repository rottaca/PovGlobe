find_package(SWIG 4.0 COMPONENTS python)


if(SWIG_FOUND)
  message("SWIG found: ${SWIG_EXECUTABLE}")
  # Make swig generate standard target names
  cmake_policy(SET CMP0078 NEW)

  INCLUDE(${SWIG_USE_FILE})
  FIND_PACKAGE(PythonLibs)
  INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
  
  set(CMAKE_SWIG_FLAGS "")
  
  set_property(SOURCE swig_wrapper.i PROPERTY CPLUSPLUS ON) 
  
  set_property(TARGET ${CORE_LIB_NAME} PROPERTY SWIG_USE_TARGET_INCLUDE_DIRECTORIES )

  swig_add_library (${PY_WRAPPER_LIB_NAME} TYPE SHARED LANGUAGE python SOURCES swig_wrapper.i ) 
  swig_link_libraries (${PY_WRAPPER_LIB_NAME} PUBLIC ${PYTHON_LIBRARIES} ${CORE_LIB_NAME} ${ENV_LIB_TARGET_NAME} ${APPS_LIB_NAME})
  
  get_target_property(_compile_defs ${ENV_LIB_TARGET_NAME} COMPILE_DEFINITIONS)
  get_target_property(MY_INCLUDES ${CORE_LIB_NAME} INCLUDE_DIRECTORIES)

  set_property(TARGET ${PY_WRAPPER_LIB_NAME} PROPERTY SWIG_INCLUDE_DIRECTORIES ${MY_INCLUDES})
  set_property(TARGET ${PY_WRAPPER_LIB_NAME} PROPERTY SWIG_COMPILE_DEFINITIONS ${_compile_defs})

endif()

