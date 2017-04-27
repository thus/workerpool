include_directories ("${PROJECT_SOURCE_DIR}/src")

add_executable(example1 example1.c)
target_link_libraries(example1 workerpool)
target_link_libraries(example1 pthread)
