PKG_SEARCH_MODULE(CONFIGXX libconfig++)

add_executable(mcdupsx main.cpp mcdupsx.cpp)
target_link_libraries (mcdupsx PRIVATE mcdu psx)
target_compile_features(mcdupsx PRIVATE cxx_delegating_constructors cxx_unicode_literals)
target_include_directories(mcdupsx PRIVATE ${CONFIGXX_INCLUDE_DIRS})
target_link_libraries(mcdupsx PRIVATE ${CONFIGXX_LIBRARIES})