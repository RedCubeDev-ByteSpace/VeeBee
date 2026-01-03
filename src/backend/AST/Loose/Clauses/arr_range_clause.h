//
// Created by ds on 1/3/26.
//

#ifndef ARR_RANGE_CLAUSE_H
#define ARR_RANGE_CLAUSE_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Array Range' clause
// --------------------
// Example:
// ] 1 To 100

typedef struct LS_ARR_RANGE_CLAUSE_NODE {
    ls_ast_node_t base;

    token_t *ltLBound;
    token_t *kwTo;
    token_t *ltUBound;

} ls_arr_range_clause_node_t;

#endif //ARR_RANGE_CLAUSE_H
