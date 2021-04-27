
if(UNIX)
    message(STATUS "Building hw application.")
    set(SOURCE_FILES_HW
        renderer_led_strip_pico.cpp
    )
    # hw library
    add_library(${HW_LIB_EXT_NAME} ${SOURCE_FILES_HW})
    target_compile_definitions(${HW_LIB_EXT_NAME} PUBLIC HW_AVAILABLE)
    target_link_libraries(${HW_LIB_EXT_NAME} PUBLIC ${CORE_LIB_NAME} pthread atomic)

endif()


