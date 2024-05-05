add_library(Utilities STATIC strings.h strings.c)
target_link_libraries(Utilities PUBLIC cxx_setup)
target_include_directories(Utilities PUBLIC ${PROJECT_SOURCE_DIR})