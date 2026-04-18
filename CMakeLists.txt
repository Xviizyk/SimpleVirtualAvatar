cmake_minimum_required (VERSION 3.24)

project ("SVA" LANGUAGES CXX)

set(PA_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(PA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(PA_BUILD_TESTS OFF CACHE BOOL "" FORCE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/portaudio)

target_link_libraries(avatar_tool PRIVATE portaudio)

target_include_directories(avatar_tool PRIVATE external/portaudio/include)

add_executable(Program src/main.cpp)