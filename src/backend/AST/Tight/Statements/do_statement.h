//
// Created by ds on 2/5/26.
//

#ifndef TG_DO_STATEMENT_H
#define TG_DO_STATEMENT_H
#include "AST/Tight/tg_ast.h"
#include "AST/Tight/Symbols/label_symbol.h"

typedef enum DO_STATEMENT_KIND {
    DO_STATEMENT_INFINITE,
    DO_STATEMENT_WHILE,
    DO_STATEMENT_UNTIL,
    DO_STATEMENT_HEAD,
    DO_STATEMENT_FOOT,
} do_statement_kind_t;

typedef struct TG_DO_STATEMENT {
    tg_ast_node_t base;

    do_statement_kind_t kindOfDo;
    do_statement_kind_t controlOfDo;

    label_symbol_t *continueLabel;
    label_symbol_t *breakLabel;

    tg_ast_node_t *condition;
    tg_ast_node_list_t statements;

} tg_do_statement_t;

#endif //TG_DO_STATEMENT_H
