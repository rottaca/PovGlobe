
set(SOURCE_FILES
	"globe.cpp"
	"application_base.cpp"
	"rpm_measure_base.cpp"
	"renderer_base.cpp"
	"helper.cpp"
)

# core library
add_library(${CORE_LIB_NAME} ${SOURCE_FILES})

# Link 
find_package(ImageMagick 6 EXACT REQUIRED COMPONENTS Magick++)

target_link_libraries(${CORE_LIB_NAME} cimg ${ImageMagick_LIBRARIES})
target_include_directories(${CORE_LIB_NAME} PUBLIC .. ${ImageMagick_INCLUDE_DIRS})

target_compile_options(${CORE_LIB_NAME} PUBLIC -fPIC)
target_compile_definitions(${CORE_LIB_NAME} PUBLIC cimg_use_magick=1 MAGICKCORE_QUANTUM_DEPTH=16 MAGICKCORE_HDRI_ENABLE=0)
