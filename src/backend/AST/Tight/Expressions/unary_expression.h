//
// Created by ds on 2/5/26.
//

#ifndef TG_UNARY_EXPRESSION_H
#define TG_UNARY_EXPRESSION_H
#include "AST/Tight/tg_ast.h"

#define FOREACH_UNARY_OPERATOR(GEN) \
    GEN(TG_OP_IDENTITY)             \
    GEN(TG_OP_NEGATION)             \
    GEN(TG_OP_LOGICAL_NEGATION)     \

#define GEN_ENUM(ENUM) ENUM,
#define GEN_STRING(STRING) #STRING,

typedef enum TG_UNARY_OPERATOR {
    FOREACH_UNARY_OPERATOR(GEN_ENUM)
} tg_unary_operator_t;

static const char *UNARY_OPERATOR_STRING[] = {
    FOREACH_UNARY_OPERATOR(GEN_STRING)
};

typedef struct TG_UNARY_EXPRESSION {
    tg_ast_node_t base;

    tg_unary_operator_t op;
    tg_ast_node_t *operand;

} tg_unary_expression_t;

#endif //TG_UNARY_EXPRESSION_H
