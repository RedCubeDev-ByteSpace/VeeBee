//
// Created by ds on 1/14/26.
//

#ifndef CONDITIONAL_CLAUSE_H
#define CONDITIONAL_CLAUSE_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Conditional' clause
// --------------------
// Example:
// ] If <condition> Then
// ] ...
// ] End If
//
// Example:
// ] ElseIf <condition> Then
// ] ...
// ] End If

typedef struct LS_CONDITIONAL_CLAUSE_NODE {
    ls_ast_node_t base;

    token_t *kwConditional;
    ls_ast_node_t *exprCondition;
    token_t *kwThen;

    ls_ast_node_list_t lsStatements;

} ls_conditional_clause_node_t;

#endif //CONDITIONAL_CLAUSE_H
