//
// Created by ds on 1/14/26.
//

#ifndef ELSE_CLAUSE_H
#define ELSE_CLAUSE_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Conditional' clause
// --------------------
// Example:
// ] Else
// ] ...
// ] End If

typedef struct LS_ELSE_CLAUSE_NODE {
    ls_ast_node_t base;

    token_t *kwElse;
    ls_ast_node_list_t lsStatements;

} ls_else_clause_node_t;

#endif //ELSE_CLAUSE_H
