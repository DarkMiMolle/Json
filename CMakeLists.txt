cmake_minimum_required(VERSION 3.8)

project(Json)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_library(Json Json.cpp include/Json.hpp)

add_subdirectory(test)

enable_testing()

add_test(NAME JsonTests WORKING_DIRECTORY test/ COMMAND JsonTest)
