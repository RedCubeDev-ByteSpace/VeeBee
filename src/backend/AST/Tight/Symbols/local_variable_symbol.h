//
// Created by ds on 1/29/26.
//

#ifndef LOCAL_VARIABLE_SYMBOL_H
#define LOCAL_VARIABLE_SYMBOL_H
#include "../tg_ast.h"
#include "type_symbol.h"

typedef struct LOCAL_VARIABLE_SYMBOL {
    symbol_t base;

    // the type of this local variable
    type_symbol_t *symType;

    // the bucket index of this variable
    uint16_t bucketIndex;

} local_variable_symbol_t;

#endif //LOCAL_VARIABLE_SYMBOL_H
