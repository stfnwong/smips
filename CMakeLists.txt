cmake_minimum_required(VERSION 3.21)

# ==== Project ==== #
project(
    smips,
    VERSION 0.1.0
    DESCRIPTION "A cycle-accurate MIPS pipeline emulator"
    LANGUAGES CXX
)


# ==== Global settings ==== #
# These settings apply to all targets 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)    # -std=c++20 rather than -std=gnu++20

# Put executables and libraries in predictable places 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Export compile commands for clangd, vim, etc
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# ==== Build type 
# If no build type is specified then default to Release 
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Build type not set, defauling to Release")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE, CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "RelWithDebInfo" "MinSizeRel"
    )
endif()
# ==== Compiler warnings 
# Define a reusable interface target for warning flags 
# Targets opt-in by linking against this
add_library(mips_warnings INTERFACE)

target_compile_options(mips_warnings INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
    >
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4
        /WX
    >
)    
# ==== Sanitizers
add_library(mips_sanitizers INTERFACE)

target_compile_options(mips_sanitizers INTERFACE
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    >
)

target_link_options(mips_sanitizers INTERFACE
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:
        -fsanitize=address,undefined
    >
)


# ==== Dependencies


# ==== Subdirectories
add_subdirectory(src)
add_subdirectory(tools)

# Only build if this is the top-level project (not a sub-dep)
if(PROJECT_IS_TOP_LEVEL)
    enable_testing()
    add_subdirectory(tests)
endif()
