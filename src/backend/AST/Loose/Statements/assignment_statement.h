//
// Created by ds on 1/13/26.
//

#ifndef ASSIGNMENT_STATEMENT_H
#define ASSIGNMENT_STATEMENT_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Assignment' statement
// ----------------------
// Example:
// ] myVariable = 10
// ] Let iMyNumber = 10
// ] Set oMyObject = <something>

typedef struct LS_ASSIGNMENT_STATEMENT_NODE {
    ls_ast_node_t base;

    token_t *kwLet;
    token_t *kwSet;

    ls_ast_node_t *exprTarget;

    token_t *opEquals;

    ls_ast_node_t *exprValue;

} ls_assignment_statement_node_t;

#endif //ASSIGNMENT_STATEMENT_H
