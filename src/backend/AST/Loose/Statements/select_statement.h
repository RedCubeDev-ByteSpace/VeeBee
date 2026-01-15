//
// Created by ds on 1/15/26.
//

#ifndef SELECT_STATEMENT_H
#define SELECT_STATEMENT_H
#include "AST/Loose/Clauses/else_clause.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Select' statement
// ------------------
// Example:
// ] Select Case <a>
// ]   Case <b>
// ]     ...
// ]   Case <c>
// ]     ...
// ]   Case Else
// ]     ...
// ] End Select

typedef struct LS_SELECT_STATEMENT_NODE {
    ls_ast_node_t base;

    token_t *kwSelect;
    token_t *kwCase;

    ls_ast_node_t *exprTest;
    ls_ast_node_list_t lsConditionals;
    ls_else_clause_node_t *clsElse;

    token_t *kwEnd;
    token_t *kwEndSelect;

} ls_select_statement_node_t;

#endif //SELECT_STATEMENT_H
