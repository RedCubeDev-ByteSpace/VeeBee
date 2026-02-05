//
// Created by ds on 2/5/26.
//

#ifndef LABEL_SYMBOL_H
#define LABEL_SYMBOL_H
#include "AST/Tight/tg_ast.h"

typedef struct LABEL_SYMBOL {
    symbol_t base;

    uint32_t labelId;

} label_symbol_t;

#endif //LABEL_SYMBOL_H
