add_executable(mcdu main.cpp mcdulogic.cpp mcdudisplay.cpp font.cpp)
target_link_libraries (mcdu ss ${SDL2_LIBRARIES} ${SDLTTF_LIBRARIES})
target_include_directories(mcdu PRIVATE ${SDL2_INCLUDE_DIRS} ${SDLTTF_INCLUDE_DIRS})
target_compile_features(mcdu PRIVATE cxx_delegating_constructors cxx_unicode_literals) 