//
// Created by ds on 2/5/26.
//

#ifndef GOTO_STATEMENT_H
#define GOTO_STATEMENT_H
#include "AST/Tight/tg_ast.h"
#include "AST/Tight/Symbols/label_symbol.h"

typedef struct TG_GOTO_STATEMENT {
    tg_ast_node_t base;

    label_symbol_t *target;

} tg_goto_statement_t;

#endif //GOTO_STATEMENT_H
