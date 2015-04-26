add_library(mcdu mcdulogic.cpp mcdudisplay.cpp mcdupage.cpp font.cpp)
target_link_libraries (mcdu ss ${SDL2_LIBRARIES} ${SDLTTF_LIBRARIES} ${SDLIMAGE_LIBRARIES})
target_include_directories(mcdu PUBLIC ${SDL2_INCLUDE_DIRS} ${SDLTTF_INCLUDE_DIRS} ${SDLIMAGE_INCLUDE_DIRS})
target_compile_features(mcdu PRIVATE cxx_delegating_constructors cxx_unicode_literals)

add_executable(mcduboeing main.cpp mcduboeing.cpp)
target_link_libraries (mcduboeing mcdu)
target_compile_features(mcduboeing PRIVATE cxx_delegating_constructors cxx_unicode_literals)
