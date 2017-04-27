add_library(workerpool SHARED workerpool.c)
target_link_libraries(workerpool pthread)

install(TARGETS workerpool DESTINATION lib)

install(FILES "${PROJECT_SOURCE_DIR}/src/workerpool.h" DESTINATION include)
