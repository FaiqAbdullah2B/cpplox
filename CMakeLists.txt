cmake_minimum_required(VERSION 3.20)

project(cpplox
    VERSION 1.0
    DESCRIPTION "A lox language tree-walk interpreter in C++"
    LANGUAGES CXX
)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output executable to bins directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# List source files explicitly
set(SOURCES
    src/main.cpp
    src/Lox.cpp
    src/Files.cpp
    # Add more source files here as you create them
)

# Create executable
add_executable(cpplox ${SOURCES})

# Set include directories for the target (this allows #include "Lox.h")
target_include_directories(cpplox 
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

# Compiler warnings
if(MSVC)
    target_compile_options(cpplox PRIVATE /W4)
else()
    target_compile_options(cpplox PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic
    )
endif()

# Optional: Add debug symbols for Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        target_compile_options(cpplox PRIVATE /Zi)
    else()
        target_compile_options(cpplox PRIVATE -g)
    endif()
endif()