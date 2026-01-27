//
// Created by ds on 1/27/26.
//

#ifndef WHILE_STATEMENT_H
#define WHILE_STATEMENT_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'While' statement
// -----------------
// Example:
// ] While <a>
// ]  ...
// ] Wend

typedef struct LS_WHILE_STATEMENT_NODE {
    ls_ast_node_t base;

    token_t *kwWhile;
    ls_ast_node_t *exprCondition;
    ls_ast_node_list_t lsBody;
    token_t *kwWend;

} ls_while_statement_node_t;

#endif //WHILE_STATEMENT_H
