//
// Created by ds on 1/29/26.
//

#ifndef TYPE_SYMBOL_H
#define TYPE_SYMBOL_H
#include "../tg_ast.h"

typedef enum TYPE_SYMBOL_TYPE {

    TYPE_BUILTIN,      // a basic built in type like int, bool, etc.
    TYPE_ARRAY,        // an array of any other type
    TYPE_USER_DEFINED, // a user defined type

} type_symbol_type_t;

typedef struct TYPE_SYMBOL type_symbol_t;
typedef struct TYPE_SYMBOL {
    symbol_t base;

    // what sort of type is this?
    type_symbol_type_t typeOfType;

    // a sub type symbol for arrays
    type_symbol_t *symSubType;

    // index of the member defining this type
    int idxTypeMember;

    // list of fields for user defined types
    symbol_list_t lsFields;

    // the number of dimensions for an array
    uint8_t numArrayDimensions;

} type_symbol_t;

#endif //TYPE_SYMBOL_H
