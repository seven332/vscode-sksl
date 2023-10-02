set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sksl-wasi main.cpp)
target_link_libraries(sksl-wasi PRIVATE nlohmann_json)
