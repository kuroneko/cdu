add_executable(mcdu main.cpp font.cpp)
target_link_libraries (mcdu ss ${SDL2_LIBRARIES} ${SDLTTF_LIBRARIES} ${SDLGPU_LIB})
target_include_directories(mcdu PRIVATE ${SDL2_INCLUDE_DIRS} ${SDLTTF_INCLUDE_DIRS} ${SDLGPU_INC})
