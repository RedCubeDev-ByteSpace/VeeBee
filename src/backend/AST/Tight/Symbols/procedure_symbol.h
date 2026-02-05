//
// Created by ds on 1/29/26.
//

#ifndef PROCEDURE_SYMBOL_H
#define PROCEDURE_SYMBOL_H
#include "../tg_ast.h"
#include "type_symbol.h"

typedef enum PROCEDURE_VISIBILITY {

    PUBLIC,
    PRIVATE,
    FRIEND

} procedure_visibility_t;

typedef struct PROCEDURE_SYMBOL {
    symbol_t base;

    // index of the member defining this function or subroutine
    int idxMember;

    // the implementation index number of this function or subroutine within our module
    uint16_t bodyId;

    // the visibility of this function to other modules
    procedure_visibility_t visibility;

    // is this procedure static?
    bool isStatic;

    // does this procedure have variadic parameters?
    bool isVariadic;

    // a list of parameters this procedure has
    symbol_list_t lsParameters;

    // the return type of this function, if its a sub this is null
    type_symbol_t *symReturnType;

    // -----------------------------------------------------------------------------------------------------------------
    // INTERNALS

    // a list of all variables in this procedure, this includes a copy of the parameters
    symbol_list_t lsBuckets;

} procedure_symbol_t;

#endif //PROCEDURE_SYMBOL_H
