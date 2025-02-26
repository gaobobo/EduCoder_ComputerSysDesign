aux_source_directory(. sourceFiles)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/cpp-httplib)

# set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(oled_sender ${sourceFiles})
# target_link_libraries(${PROJECT_NAME} libcurl)
