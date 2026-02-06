//
// Created by ds on 2/5/26.
//

#ifndef DEFAULT_EXPRESSION_H
#define DEFAULT_EXPRESSION_H
#include "AST/Tight/tg_ast.h"
#include "AST/Tight/Symbols/type_symbol.h"

typedef struct TG_DEFAULT_EXPRESSION {
    tg_ast_node_t base;

    type_symbol_t *type;

} tg_default_expression_t;


#endif //DEFAULT_EXPRESSION_H
