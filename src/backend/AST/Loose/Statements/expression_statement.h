//
// Created by ds on 1/12/26.
//

#ifndef EXPRESSION_STATEMENT_H
#define EXPRESSION_STATEMENT_H
#include "AST/Loose/ls_ast.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Expression' statement
// ----------------------
// Example:
// ] myFunction()

typedef struct LS_EXPRESSION_STATEMENT_NODE {
    ls_ast_node_t base;
    ls_ast_node_t *exprExpression;
} ls_expression_statement_node_t;

#endif //EXPRESSION_STATEMENT_H
