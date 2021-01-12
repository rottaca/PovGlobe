find_package(SWIG 4.0 COMPONENTS python)


if(SWIG_FOUND)
  message("SWIG found: ${SWIG_EXECUTABLE}")
  # Make swig generate standard target names
  cmake_policy(SET CMP0078 NEW)

  include(${SWIG_USE_FILE})
  find_package(PythonLibs REQUIRED)
  include_directories(${PYTHON_INCLUDE_PATH})

  set(CMAKE_SWIG_FLAGS "")
  set_property(SOURCE swig_wrapper.i PROPERTY CPLUSPLUS ON)
  
  foreach(TGT ${POV_GLOBE_TARGETS})
    set_property(TARGET ${TGT} PROPERTY SWIG_USE_TARGET_INCLUDE_DIRECTORIES )
  endforeach()

  swig_add_library (${PY_WRAPPER_LIB_NAME} TYPE SHARED LANGUAGE python SOURCES swig_wrapper.i ) 
  swig_link_libraries (${PY_WRAPPER_LIB_NAME} PUBLIC ${PYTHON_LIBRARIES} ${POV_GLOBE_TARGETS})
  
  set(ALL_COMPILE_DEFS "")
  set(ALL_INCLUDES "")
  foreach(TGT ${POV_GLOBE_TARGETS})
    get_target_property(COMPILE_DEFS ${TGT} COMPILE_DEFINITIONS)
    get_target_property(INCLUDES ${TGT} INCLUDE_DIRECTORIES)
    if (INCLUDES)
      list(APPEND ALL_INCLUDES ${INCLUDES})
    endif()
    if (COMPILE_DEFS)
      list(APPEND ALL_COMPILE_DEFS ${COMPILE_DEFS})
    endif()
  endforeach()
  
  message(STATUS "ALL_COMP: ${ALL_COMPILE_DEFS}")
  message(STATUS "ALL_INC: ${ALL_INCLUDES}")
  set_property(TARGET ${PY_WRAPPER_LIB_NAME} PROPERTY SWIG_INCLUDE_DIRECTORIES ${ALL_INCLUDES})
  set_property(TARGET ${PY_WRAPPER_LIB_NAME} PROPERTY SWIG_COMPILE_DEFINITIONS ${ALL_COMPILE_DEFS})

endif()

