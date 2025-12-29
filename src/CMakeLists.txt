cmake_minimum_required(VERSION 3.30)
project(veebee C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_FLAGS -fsanitize=address)

if (DEFINED ENV{TESTING})
    add_compile_definitions(TESTING)
endif ()

# veebee backend library
add_library(veebee_backend
        backend/veebee.h
        backend/veebee.c

        # Error
        backend/Error/error.h
        backend/Error/error.c

        # Lexer
        backend/Lexer/token.h
        backend/Lexer/token.c
        backend/Lexer/source.h
        backend/Lexer/source.c
        backend/Lexer/lexer.h
        backend/Lexer/lexer.c
        backend/Debug/pretty_print.h
        backend/Debug/pretty_print.c
        backend/Debug/ansi.h
        backend/AST/Loose/ast.h
)

target_include_directories(veebee_backend
        PUBLIC
            ${PROJECT_SOURCE_DIR}/backend
)

# veebee frontend executable
add_executable(veebee
        frontend/main.c
        frontend/cli.h
)

target_link_libraries(veebee
        PRIVATE veebee_backend
)

# Testing
enable_testing()
add_subdirectory(test)