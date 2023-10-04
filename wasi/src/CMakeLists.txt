set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sksl-wasi 
    main.cpp
    lexer.cpp
    semaphore.cpp
    action/update.cpp
    action/close.cpp
    action/get_symbol.cpp
    action/format.cpp
)
target_include_directories(sksl-wasi PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sksl-wasi PRIVATE nlohmann_json skia_sksl)
