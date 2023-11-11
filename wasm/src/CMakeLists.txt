set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(sksl-wasm-lib STATIC
    lexer.cpp
    formatter.cpp
    simple_codec.cpp
    utf_offset.cpp
    action/update.cpp
    action/close.cpp
    action/format.cpp
    action/get_symbol.cpp
    action/get_token.cpp
    action/hover.cpp
    action/definition.cpp
)
target_include_directories(sksl-wasm-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sksl-wasm-lib PUBLIC skia_sksl)

add_executable(sksl-wasm main.cpp)
target_link_libraries(sksl-wasm PRIVATE sksl-wasm-lib)
target_link_options(sksl-wasm PRIVATE
    --no-entry
    -sENVIRONMENT=node
    -sMODULARIZE
    -sEXPORTED_RUNTIME_METHODS=ccall
    -sEXPORTED_FUNCTIONS=_ToUTFOffsets,_SetParamsSize,_GetParamsPtr,_GetResultPtr,_GetResultSize,_Update,_Close,_GetSymbol,_Format,_GetToken,_GetTokenRange,_Hover,_Definition
    -sALLOW_MEMORY_GROWTH=1
    -sINITIAL_MEMORY=32MB
    -sDISABLE_EXCEPTION_CATCHING=1
    -sNO_EXIT_RUNTIME=1
)

add_executable(sksl-wasm-lib-test
    formatter_test.cpp
    simple_codec_test.cpp
    utf_offset_test.cpp
)
target_link_libraries(sksl-wasm-lib-test PRIVATE sksl-wasm-lib gmock_main)
enable_testing()
include(GoogleTest)
gtest_discover_tests(sksl-wasm-lib-test)
