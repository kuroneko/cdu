add_executable(mcdupsx main.cpp mcdupsx.cpp)
target_link_libraries (mcdupsx PRIVATE mcdu psx)
target_compile_features(mcdupsx PRIVATE cxx_delegating_constructors)
