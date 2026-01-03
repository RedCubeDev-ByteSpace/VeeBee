//
// Created by ds on 12/30/25.
//

#ifndef PARAMETER_CLAUSE_H
#define PARAMETER_CLAUSE_H

#include "as_clause.h"
#include "../ls_ast.h"
#include "../../../Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Parameter' clause
// -------------------
// Example:
// ] ByRef MyParameter As String

typedef struct LS_PARAMETER_CLAUSE_NODE {
    ls_ast_node_t base;

    token_t *kwOptional;
    token_t *kwByVal;
    token_t *kwByRef;
    token_t *kwParamArray;

    token_t *idParamName;

    ls_as_clause_node_t *clsType;

    token_t *opEquals;
    ls_ast_node_t *expDefaultValue;

} ls_parameter_clause_node_t;

#endif //PARAMETER_CLAUSE_H
