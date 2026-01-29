//
// Created by ds on 1/29/26.
//

#ifndef FUNCTION_SYMBOL_H
#define FUNCTION_SYMBOL_H
#include "../tg_ast.h"
#include "type_symbol.h"

typedef struct FUNCTION_SYMBOL {
    symbol_t base;

    // the procedure index number of this function within our module
    uint16_t procedureId;

    // a list of parameters this function has
    symbol_list_t lsParameters;

    // the return type of this function
    type_symbol_t *symReturnType;

    // ------------------------------------

    // a list of all variables in this function
    symbol_list_t lsBuckets;

} function_symbol_t;

#endif //FUNCTION_SYMBOL_H
