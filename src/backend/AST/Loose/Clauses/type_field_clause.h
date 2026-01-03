//
// Created by ds on 12/30/25.
//

#ifndef TYPE_FIELD_CLAUSE_H
#define TYPE_FIELD_CLAUSE_H

#include "as_clause.h"
#include "../ls_ast.h"
#include "../../../Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Type Field' clause
// -------------------
// Example:
// ] Name As String

typedef struct LS_TYPE_FIELD_CLAUSE_NODE {
    ls_ast_node_t base;

    token_t *idFieldName;
    ls_as_clause_node_t *clsType;

} ls_type_field_clause_node_t;

#endif //TYPE_FIELD_CLAUSE_H
