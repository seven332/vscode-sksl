cmake_minimum_required(VERSION 3.25)

project(sksl-wasm CXX)

# Save compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Disable exceptions and rtti
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti")

add_subdirectory(third_party SYSTEM)
add_subdirectory(src)
