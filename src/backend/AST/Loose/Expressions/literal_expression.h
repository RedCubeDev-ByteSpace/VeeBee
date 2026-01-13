//
// Created by ds on 1/9/26.
//

#ifndef LITERAL_EXPRESSION_H
#define LITERAL_EXPRESSION_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Literal' expression
// --------------------
// Example:
// ] 10
// ] "my string"
// ] 0.1
// ] True

typedef struct LS_LITERAL_EXPRESSION_NODE {
    ls_ast_node_t base;

    token_t *ltLiteral;
} ls_literal_expression_node_t;

#endif //LITERAL_EXPRESSION_H
