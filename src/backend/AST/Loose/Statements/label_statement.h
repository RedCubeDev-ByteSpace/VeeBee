//
// Created by ds on 1/16/26.
//

#ifndef LABEL_STATEMENT_H
#define LABEL_STATEMENT_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Label' statement
// -----------------
// Example:
// ] myLabel:

typedef struct LS_LABEL_STATEMENT_NODE {
    ls_ast_node_t base;

    token_t *idLabel;

} ls_label_statement_node_t;

#endif //LABEL_STATEMENT_H
