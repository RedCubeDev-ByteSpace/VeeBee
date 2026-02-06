//
// Created by ds on 1/29/26.
//

#ifndef MODULE_SYMBOL_H
#define MODULE_SYMBOL_H
#include "../tg_ast.h"
#include "AST/Loose/ls_ast.h"
#include "Lexer/source.h"

typedef struct MODULE_SYMBOL {
    symbol_t base;

    // -----------------------------------------------------------------------------------------------------------------

    // has this module symbol been inserted automatically by the compiler?
    bool isAutoInserted;

    // the list of members we are basing this module on
    ls_ast_node_list_t lsMembers;

    // the source document our members have come from
    source_t source;

    // -----------------------------------------------------------------------------------------------------------------

    // a list of all functions and subroutines within this module
    symbol_list_t lsProcedures;

    // a list of all user defined types within this module
    symbol_list_t lsTypes;


} module_symbol_t;

#endif //MODULE_SYMBOL_H
