//
// Created by ds on 1/27/26.
//

#ifndef PARENTHESIZED_EXPRESSION_H
#define PARENTHESIZED_EXPRESSION_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Parenthesized' expression
// --------------------------
// Example:
// ] (<a>)

typedef struct LS_PARENTHESIZED_EXPRESSION_NODE {
    ls_ast_node_t base;

    token_t *pcOpeningParenthesis;
    ls_ast_node_t *exprInner;
    token_t *pcClosingParenthesis;

} ls_parenthesized_expression_node_t;

#endif //PARENTHESIZED_EXPRESSION_H
