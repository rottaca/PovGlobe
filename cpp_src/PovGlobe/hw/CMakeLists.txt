
if(UNIX)
    find_library(BCM2835_FOUND bcm2835)
    if (BCM2835_FOUND) 
        message(STATUS "Building hw application.")
        set(SOURCE_FILES_HW
            rpm_measure_hall.cpp 
            renderer_led_strip.cpp
        )
        # hw library
        add_library(${HW_LIB_EXT_NAME} ${SOURCE_FILES_HW})
        target_compile_definitions(${HW_LIB_EXT_NAME} PUBLIC HW_AVAILABLE)
        target_link_libraries(${HW_LIB_EXT_NAME} PUBLIC ${CORE_LIB_NAME} pthread pigpio atomic bcm2835)
        target_include_directories(${HW_LIB_EXT_NAME} PUBLIC ../../ext/pigpio)
    endif()
endif()
