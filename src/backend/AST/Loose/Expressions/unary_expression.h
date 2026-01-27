//
// Created by ds on 1/27/26.
//

#ifndef UNARY_EXPRESSION_H
#define UNARY_EXPRESSION_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"


// ---------------------------------------------------------------------------------------------------------------------
// 'Unary' expression
// ------------------
// Example:
// ] - <a>

typedef struct LS_UNARY_EXPRESSION_NODE {
    ls_ast_node_t base;

    token_t *opOperator;
    ls_ast_node_t *exprOperand;

} ls_unary_expression_node_t;

#endif //UNARY_EXPRESSION_H
