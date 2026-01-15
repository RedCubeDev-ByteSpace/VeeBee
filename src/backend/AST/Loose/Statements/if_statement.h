//
// Created by ds on 1/14/26.
//

#ifndef IF_STATEMENT_H
#define IF_STATEMENT_H
#include "AST/Loose/ls_ast.h"
#include "AST/Loose/Clauses/else_clause.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'If' statement
// --------------
// Example:
// ] If <a> Then
// ] ...
// ] ElseIf <b> Then
// ] ...
// ] ElseIf <c> Then
// ] ...
// ] Else
// ] ...
// ] End If

typedef struct LS_IF_STATEMENT_NODE {
    ls_ast_node_t base;

    ls_ast_node_list_t lsConditionals;
    ls_else_clause_node_t *clsElse;

    token_t *kwEnd;
    token_t *kwEndIf;

} ls_if_statement_node_t;

#endif //IF_STATEMENT_H
