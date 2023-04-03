include_directories("./")

aux_source_directory("./" util_srcs)

add_library(${PROJECT_NAME} SHARED ${util_srcs})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${xfutil_VERSION_MAJOR}.${xfutil_VERSION_MINOR} SOVERSION ${xfutil_VERSION_MAJOR})

target_link_libraries(${PROJECT_NAME} pthread)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)

