//
// Created by ds on 2/5/26.
//

#ifndef TG_REFERENCE_EXPRESSION_H
#define TG_REFERENCE_EXPRESSION_H
#include "AST/Tight/tg_ast.h"

typedef struct TG_REFERENCE_EXPRESSION {
    tg_ast_node_t base;

    tg_ast_node_t *baseExpression;
    symbol_t *linkSymbol;
    tg_ast_node_list_t arguments;

} tg_reference_expression_t;

#endif //TG_REFERENCE_EXPRESSION_H
