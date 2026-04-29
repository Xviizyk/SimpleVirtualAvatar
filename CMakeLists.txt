cmake_minimum_required(VERSION 3.25)

if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

# ──────────────────────────────────────────────────────────────────────────────
#  Project declaration
# ──────────────────────────────────────────────────────────────────────────────
project(SimpleVirtualAvatar
    VERSION     1.5.0
    LANGUAGES   C CXX
    DESCRIPTION "A simple virtual avatar rendering engine"
)

# ──────────────────────────────────────────────────────────────────────────────
#  Global standards & guards
# ──────────────────────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are not allowed.\n"
        "Please create a separate build directory:\n"
        "  cmake -B build && cmake --build build"
    )
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type specified — defaulting to Release")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        Debug Release RelWithDebInfo MinSizeRel)
endif()

# ──────────────────────────────────────────────────────────────────────────────
#  User options
# ──────────────────────────────────────────────────────────────────────────────
option(ENABLE_TRACY          "Enable Tracy profiler integration"   OFF)
option(ENABLE_X11            "Enable X11 display backend (Linux)" ON)
option(CROSS_COMPILE_ARM64   "Cross-compile for ARM64 target"     OFF)

# ──────────────────────────────────────────────────────────────────────────────
#  CPM.cmake – package manager bootstrap
# ──────────────────────────────────────────────────────────────────────────────
set(CPM_VERSION "0.40.0")
set(CPM_CACHE_FILE "${CMAKE_BINARY_DIR}/CPM_${CPM_VERSION}.cmake")

if(NOT EXISTS "${CPM_CACHE_FILE}")
    message(STATUS "Downloading CPM.cmake v${CPM_VERSION}…")
    file(DOWNLOAD
        "https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_VERSION}/CPM.cmake"
        "${CPM_CACHE_FILE}"
        EXPECTED_HASH SHA256=7b354f3a5976c4626c876850c93944e52c83ec59a159ae5de5be7983f0e17a2a
        STATUS _cpm_status
    )
    list(GET _cpm_status 0 _cpm_error_code)
    if(NOT _cpm_error_code EQUAL 0)
        list(GET _cpm_status 1 _cpm_error_msg)
        message(FATAL_ERROR "Failed to download CPM.cmake: ${_cpm_error_msg}")
    endif()
endif()

include("${CPM_CACHE_FILE}")

# ──────────────────────────────────────────────────────────────────────────────
#  Third-party dependencies
# ──────────────────────────────────────────────────────────────────────────────
CPMAddPackage(
    NAME        raylib
    VERSION     6.0
    GITHUB_REPOSITORY raysan5/raylib
    OPTIONS
        "BUILD_EXAMPLES OFF"
        "BUILD_GAMES    OFF"
        "CUSTOM_RGB_BASE_FRAMEBUFFER ON"
)

CPMAddPackage(
    NAME        miniaudio
    VERSION     0.11.25
    GITHUB_REPOSITORY mackron/miniaudio
    DOWNLOAD_ONLY YES
)

if(ENABLE_TRACY)
    CPMAddPackage(
        NAME        tracy
        VERSION     0.11.1
        GITHUB_REPOSITORY wolfpld/tracy
        OPTIONS
            "TRACY_ENABLE    ON"
            "TRACY_ON_DEMAND ON"
    )
endif()

# ──────────────────────────────────────────────────────────────────────────────
#  Main executable
# ──────────────────────────────────────────────────────────────────────────────
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.hpp"
    "src/*.h"
)

if(NOT PROJECT_SOURCES)
    message(FATAL_ERROR "No source files found under src/. Nothing to build.")
endif()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# ──────────────────────────────────────────────────────────────────────────────
#  Include paths
# ──────────────────────────────────────────────────────────────────────────────
target_include_directories(${PROJECT_NAME}
    PRIVATE
        src
    SYSTEM PRIVATE
        "${miniaudio_SOURCE_DIR}"
)

# ──────────────────────────────────────────────────────────────────────────────
#  Compiler warnings  (shared interface target keeps things DRY)
# ──────────────────────────────────────────────────────────────────────────────
add_library(project_warnings INTERFACE)

if(MSVC)
    target_compile_options(project_warnings INTERFACE
        /W4
        /FS
        /MT
        /EHsc
        /utf-8
        /permissive-
    )
else()
    target_compile_options(project_warnings INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Woverloaded-virtual
        -Wmisleading-indentation
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE project_warnings)

# ──────────────────────────────────────────────────────────────────────────────
#  Optimisation flags  (Release / RelWithDebInfo only)
# ──────────────────────────────────────────────────────────────────────────────
add_library(project_optimizations INTERFACE)

if(MSVC)
    target_compile_options(project_optimizations INTERFACE
        $<$<CONFIG:Release,RelWithDebInfo>:/O2 /Ot /Oi /GL /arch:AVX2>
    )
    target_link_options(project_optimizations INTERFACE
        $<$<CONFIG:Release,RelWithDebInfo>:/LTCG>
    )
else()
    target_compile_options(project_optimizations INTERFACE
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
        $<$<CONFIG:RelWithDebInfo>:-O2 -g -DNDEBUG>
        $<$<CONFIG:Debug>:-O0 -g>
    )

    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$" OR CROSS_COMPILE_ARM64)
        target_compile_options(project_optimizations INTERFACE
            -march=armv8-a -mtune=generic
        )
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64)$")
        target_compile_options(project_optimizations INTERFACE
            $<$<CONFIG:Release,RelWithDebInfo>:-march=native>
        )
    endif()
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE project_optimizations)

# ──────────────────────────────────────────────────────────────────────────────
#  Platform-specific linking
# ──────────────────────────────────────────────────────────────────────────────
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        winmm gdi32 opengl32 user32 shell32
    )
    if(NOT MSVC)
        target_link_options(${PROJECT_NAME} PRIVATE
            -static
            $<$<NOT:$<CONFIG:Debug>>:-mwindows>
        )
    endif()

elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework OpenGL"
    )

else()
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread dl m)

    if(ENABLE_X11)
        find_package(X11 REQUIRED COMPONENTS Xrandr Xinerama Xcursor Xi)
        target_link_libraries(${PROJECT_NAME} PRIVATE
            X11::X11
            X11::Xrandr
            X11::Xinerama
            X11::Xcursor
            X11::Xi
            GL
        )
    endif()
endif()

# ──────────────────────────────────────────────────────────────────────────────
#  Core library linkage
# ──────────────────────────────────────────────────────────────────────────────
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# ──────────────────────────────────────────────────────────────────────────────
#  Optional: Tracy profiler
# ──────────────────────────────────────────────────────────────────────────────
if(ENABLE_TRACY)
    target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
        "${tracy_SOURCE_DIR}/public"
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE TracyClient)
    target_compile_definitions(${PROJECT_NAME} PRIVATE TRACY_ENABLE)
endif()

# ──────────────────────────────────────────────────────────────────────────────
#  Install rules
# ──────────────────────────────────────────────────────────────────────────────
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ──────────────────────────────────────────────────────────────────────────────
#  Configuration summary
# ──────────────────────────────────────────────────────────────────────────────
macro(summary_row label value)
    string(LENGTH "${label}" _llen)
    string(LENGTH "${value}" _vlen)
    math(EXPR _pad "20 - ${_llen}")
    string(REPEAT " " ${_pad} _spaces)
    message(STATUS "║  ${label}${_spaces}${value}")
endmacro()

message(STATUS "")
message(STATUS "╔══════════════════════════════════════╗")
message(STATUS "║   ${PROJECT_NAME} — Build Summary")
message(STATUS "╠══════════════════════════════════════╣")
summary_row("Version"     "${PROJECT_VERSION}")
summary_row("Build type"  "${CMAKE_BUILD_TYPE}")
summary_row("Compiler"    "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
summary_row("System"      "${CMAKE_SYSTEM_NAME} / ${CMAKE_SYSTEM_PROCESSOR}")
summary_row("C++ standard" "C++${CMAKE_CXX_STANDARD}")
message(STATUS "║")
summary_row("Tracy"       "${ENABLE_TRACY}")
summary_row("X11"         "${ENABLE_X11}")
summary_row("ARM64 xcomp" "${CROSS_COMPILE_ARM64}")
message(STATUS "╚══════════════════════════════════════╝")
message(STATUS "")