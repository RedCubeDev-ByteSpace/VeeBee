//
// Created by ds on 2/5/26.
//

#ifndef TG_PROC_CALL_STATEMENT_H
#define TG_PROC_CALL_STATEMENT_H
#include "AST/Tight/tg_ast.h"
#include "AST/Tight/Symbols/procedure_symbol.h"

typedef struct TG_PROC_CALL_STATEMENT {
    tg_ast_node_t base;

    procedure_symbol_t *procedure;
    tg_ast_node_list_t arguments;

} tg_proc_call_statement_t;

#endif //TG_PROC_CALL_STATEMENT_H
