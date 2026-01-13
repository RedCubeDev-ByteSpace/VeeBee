//
// Created by ds on 1/9/26.
//

#ifndef REFERENCE_EXPRESSION_C_H
#define REFERENCE_EXPRESSION_C_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Reference' expression
// ----------------------
// Example:
// ] myVariable
// ] myVariable.myField
// ] myVariable.myMethod(a, b, c).myField

typedef struct LS_REFERENCE_EXPRESSION_NODE {
    ls_ast_node_t base;

    ls_ast_node_t *exprBase;
    token_t *idName;

    token_t *pcOpenParenthesis;
    ls_ast_node_list_t lsArguments;
    token_t *pcClosedParenthesis;

} ls_reference_expression_node_t;

#endif //REFERENCE_EXPRESSION_C_H
