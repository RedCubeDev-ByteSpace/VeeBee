//
// Created by ds on 1/27/26.
//

#ifndef EXIT_STATEMENT_H
#define EXIT_STATEMENT_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"


// ---------------------------------------------------------------------------------------------------------------------
// 'Exit' statement
// ----------------
// Example:
// ] Exit Sub

typedef struct LS_EXIT_STATEMENT_NODE {
    ls_ast_node_t base;

    token_t *kwExit;
    token_t *kwContainer;

} ls_exit_statement_node_t;

#endif //EXIT_STATEMENT_H
