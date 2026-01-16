//
// Created by ds on 1/16/26.
//

#ifndef GOTO_STATEMENT_H
#define GOTO_STATEMENT_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Goto' statement
// ----------------
// Example:
// ] GoTo myLabel

typedef struct LS_GOTO_STATEMENT_NODE {
    ls_ast_node_t base;

    token_t *kwGoto;
    token_t *idLabel;

} ls_goto_statement_node_t;

#endif //GOTO_STATEMENT_H
