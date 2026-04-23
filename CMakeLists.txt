cmake_minimum_required(VERSION 3.24)
project("SimpleVirtualAvatar" VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /FS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(RAYLIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/raylib")

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
    src/WinUtils.cpp src/WinUtils.hpp
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
    # Статическая линовка рантайма
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
message(STATUS "=========================================")
message(STATUS "")