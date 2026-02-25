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

# --- GENERATOR TOOL ---
# 1. Build the generator first
add_executable(generate_ast tools/GenerateAst.cpp)

# 2. Define where the generated headers will live
set(AST_OUT_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${AST_OUT_DIR})

# 3. Tell CMake how to create Expr.h
# We add it as a custom command that runs the generator
add_custom_command(
    OUTPUT ${AST_OUT_DIR}/Expr.h
    COMMAND generate_ast ${AST_OUT_DIR}
    DEPENDS generate_ast
    COMMENT "Generating AST classes..."
)

# --- EXECUTABLES ---

# A. The Main Interpreter (cpplox)
set(SOURCES
    src/main.cpp
    src/Lox.cpp
    src/Files.cpp
    src/Scanner.cpp
)

# We include generated headers in the source list to trigger the custom command
add_executable(cpplox ${SOURCES} ${AST_OUT_DIR}/Expr.h)

target_include_directories(cpplox 
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
        ${AST_OUT_DIR} # Allows #include "Expr.h"
)

# B. The AST Printer (Debug Tool)
# This is a separate executable that runs your AstPrinter.cpp main()
add_executable(debug_ast src/AstPrinter.cpp ${AST_OUT_DIR}/Expr.h)

target_include_directories(debug_ast 
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
        ${AST_OUT_DIR}
)

# --- COMPILER OPTIONS ---
if(MSVC)
    target_compile_options(cpplox PRIVATE /W4)
    target_compile_options(debug_ast PRIVATE /W4)
else()
    target_compile_options(cpplox PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(debug_ast PRIVATE -Wall -Wextra -Wpedantic)
endif()