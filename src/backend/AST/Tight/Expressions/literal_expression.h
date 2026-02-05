//
// Created by ds on 2/5/26.
//

#ifndef LITERAL_EXPRESSION_H
#define LITERAL_EXPRESSION_H
#include "AST/Tight/tg_ast.h"
#include "AST/Tight/Symbols/type_symbol.h"

typedef struct TG_LITERAL_EXPRESSION {
    tg_ast_node_t base;

    type_symbol_t *literalType;
    void *value;

} tg_reference_expression_t;

#endif //LITERAL_EXPRESSION_H
