cmake_minimum_required(VERSION 3.24)
project("SimpleVirtualAvatar" VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /FS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(EXISTS "/data/data/com.termux/files/usr/include/X11/Xlib.h")
    message(STATUS "Detected Termux environment with X11")
    set(TERMUX_X11_INCLUDE "/data/data/com.termux/files/usr/include")
    set(TERMUX_X11_LIB "/data/data/com.termux/files/usr/lib")
    
    set(X11_FOUND TRUE)
    set(X11_INCLUDE_DIR "${TERMUX_X11_INCLUDE}")
    set(X11_X11_INCLUDE_PATH "${TERMUX_X11_INCLUDE}")
    set(X11_X11_LIB "${TERMUX_X11_LIB}/libX11.so")
    set(X11_Xrandr_LIB "${TERMUX_X11_LIB}/libXrandr.so")
    set(X11_Xinerama_LIB "${TERMUX_X11_LIB}/libXinerama.so")
    set(X11_Xcursor_LIB "${TERMUX_X11_LIB}/libXcursor.so")
    set(X11_Xi_LIB "${TERMUX_X11_LIB}/libXi.so")
    
    include_directories(${TERMUX_X11_INCLUDE})
    link_directories(${TERMUX_X11_LIB})
endif()

set(RAYLIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/raylib")

set(GLFW_USE_WAYLAND OFF CACHE BOOL "")
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "")
set(GLFW_WAYLAND_LIBDIR "" CACHE STRING "")
set(GLFW_WAYLAND_INCLUDE_DIR "" CACHE STRING "")

if(EXISTS "${RAYLIB_PATH}/CMakeLists.txt")
    add_subdirectory("${RAYLIB_PATH}" raylib)
    set(RAYLIB_AVAILABLE TRUE)
elseif(EXISTS "${RAYLIB_PATH}/src/raylib.h")
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
    target_compile_definitions(raylib PRIVATE PLATFORM_DESKTOP GRAPHICS_API_OPENGL_33)
else()
    find_package(raylib QUIET)
    if(NOT raylib_FOUND)
        message(FATAL_ERROR "raylib not found. Clone it to external/raylib")
    endif()
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/miniaudio/miniaudio.h")
    message(FATAL_ERROR "miniaudio not found in external/miniaudio")
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
)

add_executable(SimpleVirtualAvatar ${PROJECT_SOURCES})

set_target_properties(SimpleVirtualAvatar PROPERTIES 
    WIN32_EXECUTABLE "$<NOT:$<CONFIG:Debug>>"
)

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
        winmm gdi32 opengl32 user32 shell32
    )

    if(MSVC)
        target_link_options(SimpleVirtualAvatar PRIVATE 
            "$<$<NOT:$<CONFIG:Debug>>:/SUBSYSTEM:WINDOWS>"
            "$<$<NOT:$<CONFIG:Debug>>:/ENTRY:mainCRTStartup>"
        )
    else()
        target_link_options(SimpleVirtualAvatar PRIVATE "$<$<NOT:$<CONFIG:Debug>>:-mwindows>")
    endif()

elseif(APPLE)
    target_link_libraries(SimpleVirtualAvatar PRIVATE 
        pthread "-framework Cocoa" "-framework IOKit" "-framework CoreVideo" "-framework OpenGL"
    )

else()
    target_link_libraries(SimpleVirtualAvatar PRIVATE 
        pthread dl m X11 Xrandr Xinerama Xcursor Xi GL
    )
endif()

message(STATUS "")
message(STATUS "=== SimpleVirtualAvatar Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "raylib: Available")
message(STATUS "miniaudio: Available")
if(DEFINED TERMUX_X11_INCLUDE)
    message(STATUS "Platform: Termux (X11 at ${TERMUX_X11_INCLUDE})")
endif()
message(STATUS "=========================================")
message(STATUS "")