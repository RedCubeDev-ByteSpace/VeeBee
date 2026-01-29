//
// Created by ds on 1/29/26.
//

#ifndef SUBROUTINE_SYMBOL_H
#define SUBROUTINE_SYMBOL_H
#include "../tg_ast.h"

typedef struct SUBROUTINE_SYMBOL {
    symbol_t base;

    // the procedure index number of this function within our module
    uint16_t procedureId;

    // a list of parameters this function has
    symbol_list_t lsParameters;

} subroutine_symbol_t;

#endif //SUBROUTINE_SYMBOL_H
