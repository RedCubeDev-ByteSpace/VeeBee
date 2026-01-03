//
// Created by ds on 1/3/26.
//

#ifndef DIM_FIELD_CLAUSE_H
#define DIM_FIELD_CLAUSE_H
#include "as_clause.h"
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Dim field' clause
// ------------------
// Example:
// ] Dim iMyInteger As Integer, vMyVariant, aMyArray(10) As Object

typedef struct LS_DIM_FIELD_CLAUSE_NODE {
    ls_ast_node_t base;

    token_t *idName;
    ls_as_clause_node_t *clsType;

} ls_dim_field_clause_node_t;

#endif //DIM_FIELD_CLAUSE_H
