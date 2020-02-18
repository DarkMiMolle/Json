cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(JsonTest)

add_executable(JsonTest main.cpp ../../Json.cpp ../../include/Json.hpp)