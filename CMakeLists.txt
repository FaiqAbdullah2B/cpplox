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

# Build the generator first from your single cpp file
add_executable(generate_ast tools/GenerateAst.cpp)

# Define where the generated headers will live
set(AST_OUT_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${AST_OUT_DIR})

# Tell CMake how to make Expr.h and Stmt.h
add_custom_command(
    OUTPUT ${AST_OUT_DIR}/Expr.h ${AST_OUT_DIR}/Stmt.h
    COMMAND generate_ast ${AST_OUT_DIR}
    DEPENDS generate_ast
    COMMENT "Generating AST classes..."
)
# ---------------------------------

# List source files explicitly
set(SOURCES
    src/main.cpp
    src/Lox.cpp
    src/Files.cpp
    src/Scanner.cpp
    # Add more source files here as you create them
)

# Create executable
add_executable(cpplox ${SOURCES} ${AST_OUT_DIR}/Expr.h)

# Set include directories for the target (this allows #include "Lox.h")
target_include_directories(cpplox 
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
        ${AST_OUT_DIR} # <--- This lets your parser just #include "Expr.h"
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