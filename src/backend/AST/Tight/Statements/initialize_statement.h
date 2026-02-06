//
// Created by ds on 2/5/26.
//

#ifndef TG_INITIALIZE_STATEMENT_H
#define TG_INITIALIZE_STATEMENT_H
#include "AST/Tight/tg_ast.h"
#include "AST/Tight/Symbols/local_variable_symbol.h"

typedef struct TG_INITIALIZE_STATEMENT {
    tg_ast_node_t base;

    bool preserve;
    local_variable_symbol_t *variable;
    tg_ast_node_list_t ranges;

} tg_initialize_statement_t;

#endif //TG_INITIALIZE_STATEMENT_H
