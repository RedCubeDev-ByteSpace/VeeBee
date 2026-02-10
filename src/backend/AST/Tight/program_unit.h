//
// Created by ds on 1/29/26.
//

#ifndef PROGRAM_UNIT_H
#define PROGRAM_UNIT_H
#include "tg_ast.h"

// remember which builtin type is a variant
#define IDX_BUILTIN_VARIANT 0

// ---------------------------------------------------------------------------------------------------------------------
// PROGRAM UNIT
// ------------
// contains all modules, etc, for our program
// ---------------------------------------------------------------------------------------------------------------------

typedef struct PROGRAM_UNIT {

    // a list of all built in data types
    symbol_list_t lsBuiltinTypes;

    // a list of all generated array types
    symbol_list_t lsArrayTypes;

    // a list of all modules in this compilation
    symbol_list_t lsModules;

    // a running counter for our procedures
    uint32_t procedureCounter;

    // a running counter for all external procedures
    uint32_t externalProcedureCounter;

    // all procedure bodys
    tg_ast_node_list_t *bufProcedureBodies;

} program_unit_t;

program_unit_t *BD_PROGRAM_UNIT_Init();
void BD_PROGRAM_UNIT_Unload(program_unit_t *me);
void BD_PROGRAM_UNIT_InitializeProcedureBuffer(program_unit_t *me);

#define CREATE_NEW_BUILTIN_TYPE(NAME)                                    \
    typeSymbol = malloc(sizeof(type_symbol_t));                          \
    typeSymbol->base.type = TYPE_SYMBOL;                                 \
    typeSymbol->typeOfType = TYPE_BUILTIN;                               \
    strcpy(typeSymbol->base.name, NAME);                                 \
    BD_SYMBOL_LIST_Add(&newUnit->lsBuiltinTypes, (symbol_t*)typeSymbol); \

#endif //PROGRAM_UNIT_H
