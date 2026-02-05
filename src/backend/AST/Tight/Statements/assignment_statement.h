//
// Created by ds on 2/5/26.
//

#ifndef ASSIGNMENT_STATEMENT_H
#define ASSIGNMENT_STATEMENT_H
#include "AST/Tight/tg_ast.h"

typedef struct ATG_SSIGNMENT_STATEMENT {
    tg_ast_node_t base;

    tg_ast_node_t *target;
    tg_ast_node_t *value;

} tg_assignment_statement_t;

#endif //ASSIGNMENT_STATEMENT_H
