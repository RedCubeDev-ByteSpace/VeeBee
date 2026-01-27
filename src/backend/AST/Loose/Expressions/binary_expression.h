//
// Created by ds on 1/27/26.
//

#ifndef BINARY_EXPRESSION_H
#define BINARY_EXPRESSION_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Binary' expression
// -------------------
// Example:
// ] <a> + <b>

typedef struct LS_BINARY_EXPRESSION_NODE {
    ls_ast_node_t base;

    token_t *opOperator;
    ls_ast_node_t *exprLeft;
    ls_ast_node_t *exprRight;

} ls_binary_expression_node_t;

#endif //BINARY_EXPRESSION_H
