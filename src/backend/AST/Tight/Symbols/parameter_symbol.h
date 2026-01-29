//
// Created by ds on 1/29/26.
//

#ifndef PARAMETER_SYMBOL_H
#define PARAMETER_SYMBOL_H
#include "../tg_ast.h"
#include "type_symbol.h"

typedef enum PASSING_TYPE {

    PASS_BY_VALUE,
    PASS_BY_REFERENCE,

} passing_type_t;

typedef struct LOCAL_VARIABLE_SYMBOL {
    symbol_t base;

    // the type of this parameter
    type_symbol_t *symType;

    // is it optional?
    bool isOptional;

    // how is it passed?
    passing_type_t passingType;

    // default value
    tg_ast_node_t *exprDefaultValue;

    // the bucket index of this variable
    uint16_t bucketIndex;

} local_variable_symbol_t;

#endif //PARAMETER_SYMBOL_H
