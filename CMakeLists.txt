PKG_SEARCH_MODULE(CLUTTER clutter-1.0)

add_executable(mcdu main.cpp)
target_link_libraries (mcdu ss ${CLUTTER_LIBRARIES})
target_include_directories(mcdu PRIVATE ${CLUTTER_INCLUDE_DIRS})
