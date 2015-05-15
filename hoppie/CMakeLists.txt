add_executable(mcduhoppie main.cpp mcduhoppie.cpp)
target_link_libraries (mcduhoppie mcdu)
target_compile_features(mcduhoppie PRIVATE cxx_delegating_constructors)
