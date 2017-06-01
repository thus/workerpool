# Build shared library
add_library(workerpool SHARED workerpool.c)
target_link_libraries(workerpool pthread)
install(TARGETS workerpool DESTINATION lib)

# Build static library
add_library(workerpool_static STATIC workerpool.c)
target_link_libraries(workerpool_static pthread)
set_target_properties(workerpool_static PROPERTIES OUTPUT_NAME workerpool)
install(TARGETS workerpool_static DESTINATION lib)

install(FILES "${PROJECT_SOURCE_DIR}/src/workerpool.h" DESTINATION include)
