//
// Created by ds on 2/5/26.
//

#ifndef BINARY_EXPRESSION_H
#define BINARY_EXPRESSION_H
#include "AST/Tight/tg_ast.h"

typedef enum TG_BINARY_OPERATOR {

    TG_OP_EXPONENTIATION,
    TG_OP_MULTIPLICATION,
    TG_OP_DIVISION,
    TG_OP_INTEGER_DIVISION,
    TG_OP_MODULO,
    TG_OP_ADDITION,
    TG_OP_SUBTRACTION,
    TG_OP_CONCATENATION,
    TG_OP_COMPARE_EQUAL,
    TG_OP_COMPARE_UNEQUAL,
    TG_OP_COMPARE_LESS,
    TG_OP_COMPARE_GREATER,
    TG_OP_COMPARE_LESS_OR_EQUAL,
    TG_OP_COMPARE_GREATER_OR_EQUAL,
    TG_OP_LOGICAL_AND,
    TG_OP_LOGICAL_OR,
    TG_OP_LOGICAL_XOR,

} tg_binary_operator_t;

typedef struct TG_BINARY_EXPRESSION {
    tg_ast_node_t base;

    tg_binary_operator_t op;
    tg_ast_node_t left;
    tg_ast_node_t right;

} tg_binary_expression_t;

#endif //BINARY_EXPRESSION_H
