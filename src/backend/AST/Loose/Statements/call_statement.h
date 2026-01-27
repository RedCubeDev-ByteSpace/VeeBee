//
// Created by ds on 1/27/26.
//

#ifndef CALL_STATEMENT_H
#define CALL_STATEMENT_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Call' statement
// ----------------
// Example:
// ] Call MsgBox("my message")

typedef struct LS_CALL_STATEMENT_NODE {
    ls_ast_node_t base;

    token_t *kwCall;
    ls_ast_node_t *exprCall;

} ls_call_statement_node_t;

#endif //CALL_STATEMENT_H
