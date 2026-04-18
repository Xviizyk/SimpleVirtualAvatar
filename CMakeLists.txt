cmake_minimum_required (VERSION 3.24)

project ("SVA" LANGUAGES CXX) 

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Program src/main.cpp)