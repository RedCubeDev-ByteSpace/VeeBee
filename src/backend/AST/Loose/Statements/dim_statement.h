//
// Created by ds on 1/3/26.
//

#ifndef DIM_STATEMENT_H
#define DIM_STATEMENT_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Dim' statement
// ---------------
// Example:
// ] Dim iMyInteger As Integer, vMyVariant, aMyArray(10) As Object

typedef struct LS_DIM_STATEMENT_NODE {
    ls_ast_node_t base;

    token_t *kwDim;
    ls_ast_node_list_t lsDimFields;

} ls_dim_statement_node_t;

#endif //DIM_STATEMENT_H
