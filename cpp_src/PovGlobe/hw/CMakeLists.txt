
if(UNIX)
    find_library(BCM2835_FOUND bcm2835)
    if (BCM2835_FOUND) 
      message(STATUS "Building hw application.")
      set(SOURCE_FILES_HW
          renderer_led_strip_pico.cpp
      )
      # hw library
      add_library(${HW_LIB_EXT_NAME} ${SOURCE_FILES_HW})
      target_compile_definitions(${HW_LIB_EXT_NAME} PUBLIC HW_AVAILABLE)
      target_link_libraries(${HW_LIB_EXT_NAME} PUBLIC ${CORE_LIB_NAME} pthread atomic bcm2835)
    endif()
endif()


