cmake_minimum_required(VERSION 3.30)
project(veebee C)

set(CMAKE_C_STANDARD 11)

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
)

target_include_directories(veebee_backend
        PUBLIC
            ${PROJECT_SOURCE_DIR}/backend
)

# veebee frontend executable
add_executable(veebee
        frontend/main.c
        backend/Lexer/lexer.h
)

target_link_libraries(veebee
        PRIVATE veebee_backend
)