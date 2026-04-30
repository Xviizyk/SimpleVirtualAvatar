cmake_minimum_required(VERSION 3.25)

if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

# ──────────────────────────────────────────────────────────────────────────────
#  Project declaration
# ──────────────────────────────────────────────────────────────────────────────
project(SimpleVirtualAvatar
    VERSION     2.1.0
    LANGUAGES   C CXX
    DESCRIPTION "A simple virtual avatar rendering engine"
)

if(WIN32)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

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
if(LINUX)
option(ENABLE_X11            "Enable X11 display backend (Linux)" ON)
option(CROSS_COMPILE_ARM64   "Cross-compile for ARM64 target"     OFF)
endif()

# ──────────────────────────────────────────────────────────────────────────────
#  CPM.cmake – package manager bootstrap
# ──────────────────────────────────────────────────────────────────────────────
include(cmake/CPM.cmake)

# ──────────────────────────────────────────────────────────────────────────────
#  Third-party dependencies
# ──────────────────────────────────────────────────────────────────────────────
CPMAddPackage("gh:btzy/nativefiledialog-extended@1.3.0")
CPMAddPackage("gh:raysan5/raylib#6.0")
CPMAddPackage("gh:mackron/miniaudio#0.11.25")

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

find_package(OpenGL REQUIRED)

if(OPENGL_FOUND)
    message(STATUS "OpenGL found: ${OPENGL_LIBRARIES}")
else()
    message(FATAL_ERROR "OpenGL library not found! Please install OpenGL drivers.")
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
        $<$<CONFIG:Release>:-O2 -DNDEBUG>
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
        opengl32 ole32 uuid
    )
    # target_link_libraries(${PROJECT_NAME} PRIVATE
    #     winmm gdi32 opengl32 shell32 ole32 uuid
    # )
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
target_link_libraries(${PROJECT_NAME} PRIVATE raylib nfd)

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