//
// Created by ds on 1/29/26.
//

#ifndef TYPE_FIELD_SYMBOL_H
#define TYPE_FIELD_SYMBOL_H
#include "../tg_ast.h"
#include "type_symbol.h"

typedef struct TYPE_FIELD_SYMBOL {
    symbol_t base;

    // the type of this field variable
    type_symbol_t *symType;

} type_field_symbol_t;

#endif //TYPE_FIELD_SYMBOL_H
