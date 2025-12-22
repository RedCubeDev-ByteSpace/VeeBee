//
// Created by ds on 12/21/25.
//
#include <stdbool.h>
#include <stdlib.h>
#include "token.h"

#include "Error/error.h"


// Token functions


// Token List functions
token_list_t LX_TOKEN_LIST_Init() {
    return (token_list_t) { 0 };
}

void LX_TOKEN_LIST_Unload(const token_list_t *me) {
    free(me->tokens);
}

void LX_TOKEN_LIST_add(token_list_t *me, const token_t newToken) {
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