//
// Created by ds on 2/5/26.
//

#ifndef TG_BINARY_EXPRESSION_H
#define TG_BINARY_EXPRESSION_H
#include "AST/Tight/tg_ast.h"

#define FOREACH_BINARY_OPERATOR(GEN)    \
    GEN(TG_OP_EXPONENTIATION)           \
    GEN(TG_OP_MULTIPLICATION)           \
    GEN(TG_OP_DIVISION)                 \
    GEN(TG_OP_INTEGER_DIVISION)         \
    GEN(TG_OP_MODULO)                   \
    GEN(TG_OP_ADDITION)                 \
    GEN(TG_OP_SUBTRACTION)              \
    GEN(TG_OP_CONCATENATION)            \
    GEN(TG_OP_COMPARE_EQUAL)            \
    GEN(TG_OP_COMPARE_UNEQUAL)          \
    GEN(TG_OP_COMPARE_LESS)             \
    GEN(TG_OP_COMPARE_GREATER)          \
    GEN(TG_OP_COMPARE_LESS_OR_EQUAL)    \
    GEN(TG_OP_COMPARE_GREATER_OR_EQUAL) \
    GEN(TG_OP_LOGICAL_AND)              \
    GEN(TG_OP_LOGICAL_OR)               \
    GEN(TG_OP_LOGICAL_XOR)              \

#define GEN_ENUM(ENUM) ENUM,
#define GEN_STRING(STRING) #STRING,

typedef enum TG_BINARY_OPERATOR {
    FOREACH_BINARY_OPERATOR(GEN_ENUM)
} tg_binary_operator_t;

static const char *BINARY_OPERATOR_STRING[] = {
    FOREACH_BINARY_OPERATOR(GEN_STRING)
};

typedef struct TG_BINARY_EXPRESSION {
    tg_ast_node_t base;

    tg_binary_operator_t op;
    tg_ast_node_t *left;
    tg_ast_node_t *right;

} tg_binary_expression_t;

#endif //TG_BINARY_EXPRESSION_H
