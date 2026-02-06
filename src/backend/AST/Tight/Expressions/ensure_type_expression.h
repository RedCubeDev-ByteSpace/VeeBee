//
// Created by ds on 2/5/26.
//

#ifndef TG_ENSURE_TYPE_EXPRESSION_H
#define TG_ENSURE_TYPE_EXPRESSION_H
#include "AST/Tight/tg_ast.h"
#include "AST/Tight/Symbols/type_symbol.h"

typedef struct TG_ENSURE_TYPE_EXPRESSION {
    tg_ast_node_t base;

    type_symbol_t *targetType;
    tg_ast_node_t *expression;

} tg_ensure_type_expression_t;


#endif //TG_ENSURE_TYPE_EXPRESSION_H
