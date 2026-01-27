//
// Created by ds on 12/21/25.
//
#include <stdbool.h>
#include <stdlib.h>
#include "token.h"

#include "Error/error.h"


// Token functions
int LX_TOKEN_GetBinaryOperatorPrecedence(token_type_t type) {
    switch (type) {
        case TK_OP_HAT:
            return 8;

        case TK_OP_STAR:
        case TK_OP_SLASH:
        case TK_OP_BACKSLASH:
        case TK_KW_MOD:
            return 6;

        case TK_OP_PLUS:
        case TK_OP_MINUS:
            return 5;

        case TK_OP_AND:
            return 4;

        case TK_OP_EQUALS:
        case TK_OP_UNEQUALS:
        case TK_OP_LESS:
        case TK_OP_GREATER:
        case TK_OP_LESS_EQUALS:
        case TK_OP_GREATER_EQUALS:
            return 3;

        case TK_KW_AND:
            return 2;

        case TK_KW_OR:
        case TK_KW_XOR:
            return 1;

        default:
            return 0;
    }
}

int LX_TOKEN_GetUnaryOperatorPrecedence(token_type_t type) {
    switch (type) {
        case TK_OP_PLUS:
        case TK_OP_MINUS:
        case TK_KW_NOT:
            return 7; // all unary operators are level 7, higher than all operators except for exponentiation

        default:
            return 0;
    }
}

// Token List functions
token_list_t LX_TOKEN_LIST_Init() {
    return (token_list_t) { 0 };
}

void LX_TOKEN_LIST_Unload(const token_list_t me) {
    for (int i = 0; i < me.length; ++i) {

        if (me.tokens[i].strValue != NULL) free(me.tokens[i].strValue);
        if (me.tokens[i].value != NULL) free(me.tokens[i].value);
    }

    free(me.tokens);
}

void LX_TOKEN_LIST_Add(token_list_t *me, const token_t newToken) {
    // grow the list buffer if needed
    if (!LX_TOKEN_LIST_grow(me)) {

        // aw man
        error(SUB_LEXER, ERR_INTERNAL, "token list cannot grow any larger, out of memory");
        return;
    }

    // when theres sufficient space in the buffer -> add this entry
    me->tokens[me->length] = newToken;
    me->length++;
}

bool LX_TOKEN_LIST_grow(token_list_t *me) {
    if (me->length < me->capacity) return true; // nothing to do

    // otherwise: expand the capacity and reallocate the buffer
    me->capacity += LX_TOKEN_LIST_GROWTH;
    token_t *newBuffer = realloc(me->tokens, sizeof(token_t) * me->capacity);

    // did we manage to allocate a new buffer?
    if (newBuffer == NULL) {
        // if not -> roll back the capacity and return false
        me->capacity -= LX_TOKEN_LIST_GROWTH;
        return false;
    }

    // otherwise, copy over the new buffer ptr into our list
    me->tokens = newBuffer;
    return true;
}