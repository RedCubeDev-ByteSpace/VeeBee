//
// Created by ds on 12/30/25.
//

#ifndef SUB_MEMBER_H
#define SUB_MEMBER_H

#include "../ls_ast.h"
#include "../../../Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Sub' member
// ------------
// Example:
// ] Public Sub Main()
// ] ...
// ] End Sub

typedef struct LS_SUB_MEMBER_NODE {
    ls_ast_node_t base;

    token_t *kwPublic;
    token_t *kwPrivate;
    token_t *kwFriend;

    token_t *kwStatic;

    token_t *kwSub;
    token_t *idName;

    token_t *pcOpenParenthesis;
    ls_ast_node_list_t lsParameters;
    token_t *pcClosedParenthesis;

    ls_ast_node_list_t lsSubBody;

    token_t *kwEnd;
    token_t *kwEndSub;

} ls_sub_member_node_t;

#endif //SUB_MEMBER_H
