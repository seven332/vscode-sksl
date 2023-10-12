set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(sksl-wasi-lib STATIC
    lexer.cpp
    simple_codec.cpp
    semaphore.cpp
    action/update.cpp
    action/close.cpp
    action/get_symbol.cpp
    action/format.cpp
)
target_include_directories(sksl-wasi-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sksl-wasi-lib PUBLIC nlohmann_json skia_sksl)

add_executable(sksl-wasi main.cpp)
target_link_libraries(sksl-wasi PRIVATE sksl-wasi-lib)

add_executable(sksl-wasi-lib-test
    simple_codec_test.cpp
    action/format_test.cpp
)
target_link_libraries(sksl-wasi-lib-test PRIVATE sksl-wasi-lib gmock_main)
enable_testing()
include(GoogleTest)
gtest_discover_tests(sksl-wasi-lib-test)
