//
// Created by ds on 1/3/26.
//

#ifndef REDIM_STATEMENT_H
#define REDIM_STATEMENT_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'ReDim' statement
// -----------------
// Example:
// ] ReDim Preserve aMyArray(10)

typedef struct LS_REDIM_STATEMENT_NODE {
    ls_ast_node_t base;

    token_t *kwReDim;
    token_t *kwPreserve;
    ls_ast_node_list_t lsDimFields;

} ls_redim_statement_node_t;

#endif //REDIM_STATEMENT_H
