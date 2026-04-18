cmake_minimum_required (VERSION 3.24)
project ("SimpleVirtualAvatar" VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(miniaudio INTERFACE)
target_include_directories(miniaudio INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/external/miniaudio)

add_executable(SimpleVirtualAvatar src/main.cpp src/AudioHandler.cpp src/AudioInput.cpp src/Renderer.cpp src/miniaudio.cpp)
target_link_libraries(SimpleVirtualAvatar PRIVATE miniaudio)

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
    target_link_libraries(SimpleVirtualAvatar PRIVATE winmm) 
elseif(APPLE)
    find_library(CORE_AUDIO CoreAudio REQUIRED)
    find_library(AUDIO_TOOLBOX AudioToolbox REQUIRED)
    find_library(FOUNDATION Foundation REQUIRED)
    target_link_libraries(SimpleVirtualAvatar PRIVATE ${CORE_AUDIO} ${AUDIO_TOOLBOX} ${FOUNDATION})
elseif(UNIX)
    target_link_libraries(SimpleVirtualAvatar PRIVATE pthread dl m)
endif()