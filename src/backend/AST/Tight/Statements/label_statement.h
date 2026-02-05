//
// Created by ds on 2/5/26.
//

#ifndef LABEL_STATEMENT_H
#define LABEL_STATEMENT_H
#include "AST/Tight/tg_ast.h"
#include "AST/Tight/Symbols/label_symbol.h"

typedef struct TG_LABEL_STATEMENT {
    tg_ast_node_t base;

    label_symbol_t *me;

} tg_label_statement_t;

#endif //LABEL_STATEMENT_H
