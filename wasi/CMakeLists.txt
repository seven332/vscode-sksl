cmake_minimum_required(VERSION 3.22)

project(sksl-wasi CXX)

# Disable exceptions and rtti
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti")

add_executable(sksl-wasi main.cpp)
