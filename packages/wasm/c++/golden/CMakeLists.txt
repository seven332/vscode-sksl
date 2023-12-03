set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sksl-wasm-golden-test
    golden_test.cpp
    arithmode.cpp
    destcolor.cpp
    kawase_blur_rt.cpp
    ripple_shader.cpp
)
target_link_libraries(sksl-wasm-golden-test PRIVATE sksl-wasm-lib gmock_main)
enable_testing()
include(GoogleTest)
gtest_discover_tests(sksl-wasm-golden-test)
