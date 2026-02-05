//
// Created by ds on 1/29/26.
//
#include "program_unit.h"

#include <stdlib.h>
#include <string.h>

#include "Symbols/type_symbol.h"

program_unit_t *BD_PROGRAM_UNIT_Init() {
    program_unit_t *newUnit = malloc(sizeof(program_unit_t));

    // initialize the module list
    newUnit->lsModules = BD_SYMBOL_LIST_Init();

    // initialize the built in types
    newUnit->lsBuiltinTypes = BD_SYMBOL_LIST_Init();

    type_symbol_t *typeSymbol;
    CREATE_NEW_BUILTIN_TYPE("variant");
    CREATE_NEW_BUILTIN_TYPE("boolean");
    CREATE_NEW_BUILTIN_TYPE("byte");
    CREATE_NEW_BUILTIN_TYPE("integer");
    CREATE_NEW_BUILTIN_TYPE("long");
    CREATE_NEW_BUILTIN_TYPE("longlong");
    CREATE_NEW_BUILTIN_TYPE("currency");
    CREATE_NEW_BUILTIN_TYPE("single");
    CREATE_NEW_BUILTIN_TYPE("double");
    CREATE_NEW_BUILTIN_TYPE("string");

    // initialize the array type list
    newUnit->lsArrayTypes = BD_SYMBOL_LIST_Init();

    // reset the procedure counter
    newUnit->procedureCounter = 0;

    return newUnit;
}

void BD_PROGRAM_UNIT_Unload(program_unit_t *me) {
    BD_SYMBOL_LIST_Unload(me->lsModules, true);
    BD_SYMBOL_LIST_Unload(me->lsBuiltinTypes, true);
    BD_SYMBOL_LIST_Unload(me->lsArrayTypes, true);
    free(me);
}