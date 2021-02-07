add_subdirectory(core)
add_subdirectory(apps)

set(POV_GLOBE_TARGETS "")
list(APPEND POV_GLOBE_TARGETS ${CORE_LIB_NAME})
list(APPEND POV_GLOBE_TARGETS ${APPS_LIB_NAME})

add_subdirectory(hw)
if(TARGET ${HW_LIB_EXT_NAME})
  list(APPEND POV_GLOBE_TARGETS ${HW_LIB_EXT_NAME})
endif()

add_subdirectory(sim)
if(TARGET ${SIM_LIB_EXT_NAME})
  list(APPEND POV_GLOBE_TARGETS ${SIM_LIB_EXT_NAME})
endif()
 
# executable
add_executable(${EXEC_MAIN_NAME}
	main.cpp
)
target_link_libraries(${EXEC_MAIN_NAME} ${POV_GLOBE_TARGETS})

# executable
add_executable(${EXEC_MAIN_NAME}_test_rpm
	main_test_rpm.cpp
)
target_link_libraries(${EXEC_MAIN_NAME}_test_rpm ${POV_GLOBE_TARGETS})
