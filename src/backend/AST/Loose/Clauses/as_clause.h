//
// Created by ds on 12/30/25.
//

#ifndef TYPE_CLAUSE_H
#define TYPE_CLAUSE_H

#include "../ls_ast.h"
#include "../../../Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'As' clause
// -------------------
// Example:
// ] As String

typedef struct LS_AS_CLAUSE_NODE {
    ls_ast_node_t base;

    token_t *pcOpenParenthesis;
    token_t *pcClosedParenthesis;

    token_t *kwAs;
    token_t *idType;

} ls_as_clause_node_t;

#endif //TYPE_CLAUSE_H
