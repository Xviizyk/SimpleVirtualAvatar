cmake_minimum_required(VERSION 4.0)
project("SimpleVirtualAvatar" VERSION 1.5.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CROSS_COMPILE_ARM64 "Кросс-компиляция для ARM64" OFF)
option(ENABLE_X11 "Включить поддержку X11" ON)

if(MSVC)
    add_compile_options(/W4 /FS /MT)
    add_compile_options("$<$<CONFIG:Release>:/Ot;/Oi;/GL;/arch:AVX2>")
    
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64" OR CROSS_COMPILE_ARM64)
        add_compile_options(-march=armv8-a -mtune=generic)
    endif()
endif()

set(RAYLIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/raylib")

if(EXISTS "${RAYLIB_PATH}/CMakeLists.txt")
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
    set(GLFW_JOYSTICK_LINUX OFF CACHE BOOL "" FORCE)
    
    if(CROSS_COMPILE_ARM64)
        set(GLFW_USE_WAYLAND OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
    endif()
    
    add_subdirectory("${RAYLIB_PATH}" raylib)
    
    if(TARGET raylib)
        if(NOT MSVC)
            target_compile_options(raylib PRIVATE -w)
        else()
            target_compile_options(raylib PRIVATE /W0)
        endif()
    endif()
else()
    find_package(raylib REQUIRED)
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/miniaudio/miniaudio.h")
    message(FATAL_ERROR "miniaudio.h missing. Check external/miniaudio/")
endif()

set(PROJECT_SOURCES
    src/Animation.cpp src/Animation.hpp
    src/AssetManager.cpp src/AssetManager.hpp
    src/AudioInput.cpp src/AudioInput.hpp
    src/Engine.cpp src/Engine.hpp
    src/Input.cpp src/Input.hpp
    src/main.cpp
    src/Renderer.cpp src/Renderer.hpp
    src/Utils.cpp src/Utils.hpp
    src/WinUtils.cpp src/WinUtils.hpp
)

add_executable(SimpleVirtualAvatar ${PROJECT_SOURCES})

option(ENABLE_TRACY "Enable Tracy profiling" OFF)

if(ENABLE_TRACY)
    include(FetchContent)
    FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG        v0.13.1
    )
    FetchContent_MakeAvailable(tracy)

    set(TRACY_CLIENT_SOURCE "${tracy_SOURCE_DIR}/public/TracyClient.cpp")

    target_sources(SimpleVirtualAvatar PRIVATE ${TRACY_CLIENT_SOURCE})

    target_include_directories(SimpleVirtualAvatar PRIVATE "${tracy_SOURCE_DIR}/public")
    
    target_compile_definitions(SimpleVirtualAvatar PRIVATE TRACY_ENABLE)
endif()

target_include_directories(SimpleVirtualAvatar PRIVATE src)
target_include_directories(SimpleVirtualAvatar SYSTEM PRIVATE
    ${RAYLIB_PATH}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/miniaudio
)

target_link_libraries(SimpleVirtualAvatar PRIVATE raylib)

if(WIN32)
    target_link_libraries(SimpleVirtualAvatar PRIVATE winmm gdi32 opengl32 user32 shell32)
    if(NOT MSVC)
        target_link_options(SimpleVirtualAvatar PRIVATE -static -static-libgcc -static-libstdc++)
        target_link_options(SimpleVirtualAvatar PRIVATE "$<$<NOT:$<CONFIG:Debug>>:-mwindows>")
    endif()

elseif(APPLE)
    target_link_libraries(SimpleVirtualAvatar PRIVATE pthread "-framework Cocoa" "-framework IOKit" "-framework CoreVideo" "-framework OpenGL")

else()
    target_link_libraries(SimpleVirtualAvatar PRIVATE pthread dl m)
    
    if(ENABLE_X11)
        find_package(X11 REQUIRED)
        if(X11_FOUND)
            target_link_libraries(SimpleVirtualAvatar PRIVATE ${X11_LIBRARIES})
            target_include_directories(SimpleVirtualAvatar SYSTEM PRIVATE ${X11_INCLUDE_DIR})
            message(STATUS "X11 найден и включен")
        else()
            message(WARNING "X11 не найден. Приложение может не работать на Linux без X11")
        endif()
    endif()
    
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux" OR NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
        target_link_libraries(SimpleVirtualAvatar PRIVATE Xrandr Xinerama Xcursor Xi GL)
    else()
        target_link_libraries(SimpleVirtualAvatar PRIVATE GL)
        
        find_library(XRANDR_LIB Xrandr)
        find_library(XINERAMA_LIB Xinerama)
        find_library(XCURSOR_LIB Xcursor)
        find_library(XI_LIB Xi)
        
        if(XRANDR_LIB)
            target_link_libraries(SimpleVirtualAvatar PRIVATE ${XRANDR_LIB})
        endif()
        if(XINERAMA_LIB)
            target_link_libraries(SimpleVirtualAvatar PRIVATE ${XINERAMA_LIB})
        endif()
        if(XCURSOR_LIB)
            target_link_libraries(SimpleVirtualAvatar PRIVATE ${XCURSOR_LIB})
        endif()
        if(XI_LIB)
            target_link_libraries(SimpleVirtualAvatar PRIVATE ${XI_LIB})
        endif()
    endif()
endif()

message(STATUS "")
message(STATUS "╔════════════════════════════════════════════════════════╗")
message(STATUS "║         SimpleVirtualAvatar Configuration              ║")
message(STATUS "╠════════════════════════════════════════════════════════╣")
message(STATUS "│ Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "│ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "│ C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "│")
message(STATUS "│ Platform:")

if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "│   Windows x86_64")
    else()
        message(STATUS "│   Windows ARM64")
    endif()
elseif(APPLE)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "│   macOS x86_64")
    else()
        message(STATUS "│   macOS ARM64")
    endif()
else()
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "│   Linux x86_64")
    else()
        message(STATUS "│   Linux ARM64")
    endif()
    if(ENABLE_X11)
        message(STATUS "│   X11: ✓ Enabled")
    else()
        message(STATUS "│   X11: ✗ Disabled")
    endif()
endif()

message(STATUS "│")
message(STATUS "│ Libraries:")
message(STATUS "│   raylib: Available")
message(STATUS "│   miniaudio: Available")

if(ENABLE_X11 AND NOT WIN32 AND NOT APPLE AND X11_FOUND)
    message(STATUS "│   X11: Available")
endif()

message(STATUS "│")
if(CROSS_COMPILE_ARM64)
    message(STATUS "│ Cross-compilation: ARM64")
    message(STATUS "│ Flags: -march=armv8-a -mtune=generic")
endif()

message(STATUS "╚════════════════════════════════════════════════════════╝")
message(STATUS "")