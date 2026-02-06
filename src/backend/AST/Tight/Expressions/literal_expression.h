//
// Created by ds on 2/5/26.
//

#ifndef TG_LITERAL_EXPRESSION_H
#define TG_LITERAL_EXPRESSION_H
#include "AST/Tight/tg_ast.h"
#include "AST/Tight/Symbols/type_symbol.h"

typedef struct TG_LITERAL_EXPRESSION {
    tg_ast_node_t base;

    type_symbol_t *literalType;
    void *value;

} tg_literal_expression_t;

#endif //TG_LITERAL_EXPRESSION_H
