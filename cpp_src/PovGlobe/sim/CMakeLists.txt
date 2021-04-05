
message(STATUS "Building simulation framework.")

set(SOURCE_FILES_SIM 
	"renderer_sim.cpp"
)

# sim library
add_library(${SIM_LIB_EXT_NAME} ${SOURCE_FILES_SIM})
target_compile_definitions(${SIM_LIB_EXT_NAME} PUBLIC SIM_AVAILABLE)

if (WIN32)
  target_link_libraries(${SIM_LIB_EXT_NAME} PUBLIC ${CORE_LIB_NAME} gdi32)
elseif (UNIX)
  find_package(X11 REQUIRED)
  target_link_libraries(${SIM_LIB_EXT_NAME} PUBLIC ${CORE_LIB_NAME} ${X11_LIBRARIES} pthread atomic)
endif()