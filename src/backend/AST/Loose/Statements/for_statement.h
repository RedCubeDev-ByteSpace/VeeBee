//
// Created by ds on 1/27/26.
//

#ifndef FOR_STATEMENT_H
#define FOR_STATEMENT_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'For' statement
// ---------------
// Example:
// ] For <a> = <b> To <c> Step <d>
// ] ...
// ] Next

typedef struct LS_FOR_STATEMENT_NODE {
    ls_ast_node_t base;

    token_t *kwFor;
    token_t *idIterator;
    token_t *opEquals;
    ls_ast_node_t *exprStart;

    token_t *kwTo;
    ls_ast_node_t *exprEnd;

    token_t *kwStep;
    ls_ast_node_t *exprStep;

    ls_ast_node_list_t lsBody;

    token_t *kwNext;

} ls_for_statement_node_t;

#endif //FOR_STATEMENT_H
