
set(APP_SOURCE_FILES
	"application_examples.cpp"
)


# Applications
add_library(${APPS_LIB_NAME}
	${APP_SOURCE_FILES}
)
target_link_libraries(${APPS_LIB_NAME} ${ENV_LIB_TARGET_NAME} ${CORE_LIB_NAME})
target_include_directories(${APPS_LIB_NAME} PUBLIC ${ImageMagick_INCLUDE_DIRS})