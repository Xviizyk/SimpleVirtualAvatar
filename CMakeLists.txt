cmake_minimum_required(VERSION 3.24)
project("SimpleVirtualAvatar" VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
    add_compile_options(/FS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(RAYLIB_VERSION 5.0)
set(RAYLIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/raylib")

if(EXISTS "${RAYLIB_PATH}/CMakeLists.txt")
    message(STATUS "Building raylib from source (CMakeLists.txt found)...")
    add_subdirectory("${RAYLIB_PATH}" raylib)
    set(RAYLIB_AVAILABLE TRUE)
elseif(EXISTS "${RAYLIB_PATH}/src/raylib.h")
    message(STATUS "Building raylib from source files...")
    set(RAYLIB_SRC
        ${RAYLIB_PATH}/src/rcore.c
        ${RAYLIB_PATH}/src/rshapes.c
        ${RAYLIB_PATH}/src/rtextures.c
        ${RAYLIB_PATH}/src/rtext.c
        ${RAYLIB_PATH}/src/rmodels.c
        ${RAYLIB_PATH}/src/raudio.c
    )
    
    add_library(raylib STATIC ${RAYLIB_SRC})
    target_include_directories(raylib PRIVATE ${RAYLIB_PATH}/src)
    target_compile_definitions(raylib PRIVATE 
        PLATFORM_DESKTOP 
        GRAPHICS_API_OPENGL_33
    )
    set(RAYLIB_AVAILABLE TRUE)
else()
    message(WARNING "raylib not found in external/raylib. Trying to find installed raylib...")
    find_package(raylib QUIET)
    if(raylib_FOUND)
        message(STATUS "Found installed raylib")
        set(RAYLIB_AVAILABLE TRUE)
    else()
        message(FATAL_ERROR "raylib not found. Please clone raylib repository:
            git clone https://github.com/raysan5/raylib.git external/raylib")
    endif()
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/miniaudio/miniaudio.h")
    message(STATUS "miniaudio found")
else()
    message(FATAL_ERROR "miniaudio not found. Please clone miniaudio repository:
        git clone https://github.com/mackron/miniaudio.git external/miniaudio")
endif()

set(PROJECT_SOURCES
    src/Animation.cpp
    src/Animation.hpp
    src/AssetManager.cpp
    src/AssetManager.hpp
    src/AudioInput.cpp
    src/AudioInput.hpp
    src/Engine.cpp
    src/Engine.hpp
    src/Input.hpp
    src/main.cpp
    src/Renderer.cpp
    src/Renderer.hpp
    src/Utils.hpp
    src/WinUtils.cpp
    src/WinUtils.hpp
)

add_executable(SimpleVirtualAvatar ${PROJECT_SOURCES})

target_include_directories(SimpleVirtualAvatar PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/external/raylib/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/miniaudio
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(SimpleVirtualAvatar PRIVATE 
    PLATFORM_DESKTOP 
    GRAPHICS_API_OPENGL_33
)

target_link_libraries(SimpleVirtualAvatar PRIVATE raylib)

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
    target_link_libraries(SimpleVirtualAvatar PRIVATE 
        winmm 
        gdi32 
        opengl32
        user32
        shell32
    )
elseif(APPLE)
    target_link_libraries(SimpleVirtualAvatar PRIVATE 
        pthread 
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework OpenGL"
    )
else()
    target_link_libraries(SimpleVirtualAvatar PRIVATE 
        pthread 
        dl 
        m
        X11
        Xrandr
        Xinerama
        Xcursor
        Xi
        GL
    )
endif()

message(STATUS "")
message(STATUS "=== SimpleVirtualAvatar Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "raylib: Available")
message(STATUS "miniaudio: Available")
message(STATUS "=========================================")
message(STATUS "")