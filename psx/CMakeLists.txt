add_executable(mcdupsx main.cpp mcdupsx.cpp)
target_link_libraries (mcdupsx mcdu psx config++)
target_compile_features(mcdupsx PRIVATE cxx_delegating_constructors cxx_unicode_literals)
