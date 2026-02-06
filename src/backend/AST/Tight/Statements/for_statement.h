//
// Created by ds on 2/5/26.
//

#ifndef TG_FOR_STATEMENT_H
#define TG_FOR_STATEMENT_H
#include "AST/Tight/tg_ast.h"
#include "AST/Tight/Symbols/label_symbol.h"

typedef struct TG_FOR_STATEMENT {
    tg_ast_node_t base;

    tg_ast_node_t *initializer;
    tg_ast_node_t *upperBound;
    tg_ast_node_t *step;

    label_symbol_t *continueLabel;
    label_symbol_t *breakLabel;

    tg_ast_node_list_t statements;

} tg_for_statement_t;

#endif //TG_FOR_STATEMENT_H
