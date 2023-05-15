include_directories(${PROJECT_SOURCE_DIR}/libhello)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(hello main.c)
target_link_libraries(hello libhello)
