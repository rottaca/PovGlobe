
set(SOURCE_FILES_HW
	src/hw/rpm_measure_hall.cpp 
	src/hw/renderer_led_strip.cpp
)

# hw library
add_library(${HW_LIB_EXT_NAME} ${SOURCE_FILES_HW})
target_link_libraries(${HW_LIB_EXT_NAME} ${CORE_LIB_NAME} pthread ws2811 pigpio atomic)
target_include_directories(${HW_LIB_EXT_NAME} PUBLIC ext/pigpio ext/rpi_ws281x)