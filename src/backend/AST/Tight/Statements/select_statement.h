//
// Created by ds on 2/5/26.
//

#ifndef SELECT_STATEMENT_H
#define SELECT_STATEMENT_H
#include "AST/Tight/tg_ast.h"

typedef struct TG_SELECT_STATEMENT {
    tg_ast_node_t base;

    tg_ast_node_list_t branchConditions;
    tg_ast_node_list_list_t branchStatements;
    tg_ast_node_list_t elseStatements;

} tg_select_statement_t;

#endif //SELECT_STATEMENT_H
