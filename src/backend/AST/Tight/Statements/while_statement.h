//
// Created by ds on 2/5/26.
//

#ifndef TG_WHILE_STATEMENT_H
#define TG_WHILE_STATEMENT_H
#include "AST/Tight/tg_ast.h"
#include "AST/Tight/Symbols/label_symbol.h"

typedef struct TG_WHILE_STATEMENT {
    tg_ast_node_t base;

    tg_ast_node_t *condition;
    label_symbol_t *continueLabel;

    tg_ast_node_list_t *statements;

} tg_while_statement_t;

#endif //TG_WHILE_STATEMENT_H
