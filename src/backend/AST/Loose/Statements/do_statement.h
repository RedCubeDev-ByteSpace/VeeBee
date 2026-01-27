//
// Created by ds on 1/27/26.
//

#ifndef DO_STATEMENT_H
#define DO_STATEMENT_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Do' statement
// --------------
// Example:
// ] Do While <a>
// ]  ...
// ] Loop
//
// ] Do
// ]  ...
// ] Loop While <a>

typedef struct LS_DO_STATEMENT_NODE {
    ls_ast_node_t base;

    token_t *kwDo;
    token_t *kwHeadWhile;
    token_t *kwHeadUntil;

    ls_ast_node_t *exprCondition;
    ls_ast_node_list_t lsBody;

    token_t *kwLoop;

    token_t *kwTailWhile;
    token_t *kwTailUntil;
    
} ls_do_statement_node_t;

#endif //DO_STATEMENT_H
