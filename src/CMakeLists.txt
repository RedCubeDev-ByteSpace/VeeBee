cmake_minimum_required(VERSION 3.30)
project(veebee C)

set(CMAKE_C_STANDARD 23)
#set(CMAKE_C_FLAGS -fsanitize=address)
#SET(CMAKE_BUILD_TYPE Debug)

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
        backend/AST/Loose/ls_ast.h
        backend/AST/Loose/ls_ast.c
        backend/AST/Loose/Members/module_member.h
        backend/AST/Loose/Members/type_member.h
        backend/AST/Loose/Members/function_member.h
        backend/AST/Loose/Clauses/as_clause.h
        backend/AST/Loose/Clauses/type_field_clause.h
        backend/AST/Loose/Clauses/parameter_clause.h
        backend/AST/Loose/Members/sub_member.h
        backend/Parser/parser.h
        backend/Parser/parser.c
        backend/AST/Loose/Clauses/arr_range_clause.h
        backend/AST/Loose/Statements/dim_statement.h
        backend/AST/Loose/Clauses/dim_field_clause.h
        backend/AST/Loose/Statements/redim_statement.h
        backend/AST/Loose/Expressions/literal_expression.h
        backend/AST/Loose/Expressions/reference_expression.h
        backend/AST/Loose/Statements/expression_statement.h
        backend/AST/Loose/Statements/assignment_statement.h
        backend/AST/Loose/Clauses/conditional_clause.h
        backend/AST/Loose/Clauses/else_clause.h
        backend/AST/Loose/Statements/if_statement.h
        backend/AST/Loose/Statements/select_statement.h
        backend/AST/Loose/Statements/goto_statement.h
        backend/AST/Loose/Statements/label_statement.h
        backend/AST/Loose/Statements/for_statement.h
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