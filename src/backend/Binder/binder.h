//
// Created by ds on 1/29/26.
//

#ifndef BINDER_H
#define BINDER_H
#include "AST/Tight/program_unit.h"
#include "AST/Tight/tg_ast.h"
#include "AST/Tight/Symbols/type_symbol.h"
#include "Lexer/source.h"

#define MAX_ARRAY_DIMENSIONS 60

typedef struct BINDER {

    // have we hit an error while binding?
    bool hasError;

    // the program unit we are building
    program_unit_t *programUnit;

} binder_t;

binder_t *BINDER_Init();

#endif //BINDER_H
