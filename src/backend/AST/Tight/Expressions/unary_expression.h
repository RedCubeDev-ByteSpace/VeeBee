//
// Created by ds on 2/5/26.
//

#ifndef TG_UNARY_EXPRESSION_H
#define TG_UNARY_EXPRESSION_H
#include "AST/Tight/tg_ast.h"


typedef enum TG_UNARY_OPERATOR {

    TG_OP_IDENTITY,
    TG_OP_NEGATION,
    TG_OP_LOGICAL_NEGATION,

} tg_unary_operator_t;

typedef struct TG_UNARY_EXPRESSION {
    tg_ast_node_t base;

    tg_unary_operator_t op;
    tg_ast_node_t *operand;

} tg_unary_expression_t;

#endif //TG_UNARY_EXPRESSION_H
