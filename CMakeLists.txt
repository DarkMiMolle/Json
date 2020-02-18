cmake_minimum_required(VERSION 3.8)

project(Json)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



add_library(Json Json.cpp include/Json.hpp)