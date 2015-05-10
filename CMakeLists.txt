cmake_minimum_required(VERSION 3.2)
project(mcdu VERSION 0.1)

# Find SDL2
INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 sdl2)
PKG_SEARCH_MODULE(SDLTTF SDL2_ttf)
PKG_SEARCH_MODULE(SDLIMAGE SDL2_image)
PKG_SEARCH_MODULE(CONFIGXX libconfig++)

add_library(mcdu src/config.cpp src/mcdulogic.cpp src/mcdudisplay.cpp src/mcdupage.cpp src/font.cpp src/smartlogic.cpp)
target_link_libraries (mcdu ${SDL2_LIBRARIES} ${SDLTTF_LIBRARIES} ${SDLIMAGE_LIBRARIES} ${CONFIGXX_LIBRARIES})
target_include_directories(mcdu PUBLIC ${SDL2_INCLUDE_DIRS} ${SDLTTF_INCLUDE_DIRS} ${SDLIMAGE_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include ${CONFIGXX_INCLUDE_DIRS})
target_compile_features(mcdu PRIVATE cxx_nonstatic_member_init cxx_delegating_constructors cxx_unicode_literals)

# Precision Simulator X CDU
add_subdirectory(libpsx)
add_subdirectory(psx)

# Generic Hoppie/ACARS CDU (incomplete)
add_subdirectory(hoppie)
