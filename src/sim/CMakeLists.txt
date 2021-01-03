
set(SOURCE_FILES_SIM 
	"rpm_measure_sim.cpp"  
	"renderer_sim.cpp"
)

# sim library
add_library(${SIM_LIB_EXT_NAME} ${SOURCE_FILES_SIM})
if (WIN32)
  target_link_libraries(${SIM_LIB_EXT_NAME} ${CORE_LIB_NAME} gdi32)
elseif (UNIX)
  find_package(X11 REQUIRED)
  target_link_libraries(${SIM_LIB_EXT_NAME} ${CORE_LIB_NAME} ${X11_LIBRARIES} pthread atomic)
endif()