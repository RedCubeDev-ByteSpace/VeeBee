//
// Created by ds on 12/30/25.
//

#ifndef FUNCTION_MEMBER_H
#define FUNCTION_MEMBER_H

#include "../ls_ast.h"
#include "../Clauses/as_clause.h"
#include "../../../Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Function' member
// ---------------
// Example:
// ] Public Function MyFunction(Param As Integer) As String
// ] ...
// ] End Function

typedef struct LS_FUNCTION_MEMBER_NODE {
    ls_ast_node_t base;

    token_t *kwPublic;
    token_t *kwPrivate;
    token_t *kwFriend;

    token_t *kwStatic;

    token_t *kwFunction;
    token_t *idName;

    token_t *pcOpenParenthesis;
    ls_ast_node_list_t lsParameters;
    token_t *pcClosedParenthesis;

    ls_as_clause_node_t *clsReturnType;

    ls_ast_node_list_t lsFunctionBody;

    token_t *kwEnd;
    token_t *kwEndFunction;

} ls_function_member_node_t;

#endif //FUNCTION_MEMBER_H
