//
// Created by ds on 12/30/25.
//

#ifndef TYPE_MEMBER_H
#define TYPE_MEMBER_H

#include "../ls_ast.h"
#include "../../../Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Type' member
// ---------------
// Example:
// ] Type Employee
// ]   Name As String
// ]   BirthYear As Integer
// ] End Type

typedef struct LS_TYPE_MEMBER_NODE {
    ls_ast_node_t base;

    token_t *kwType;
    token_t *idTypeName;

    ls_ast_node_list_t lsFields;

    token_t *kwEnd;
    token_t *kwEndType;

} ls_type_member_node_t;

#endif //TYPE_MEMBER_H
