cmake_minimum_required(VERSION 3.19)
project(perfaware_programming)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

add_executable(generator src/generator/main.cpp)
add_executable(processor src/main.cpp src/json.cpp)
