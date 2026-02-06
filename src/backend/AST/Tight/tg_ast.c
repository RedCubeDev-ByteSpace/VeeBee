//
// Created by ds on 1/28/26.
//
#include <stdlib.h>
#include "tg_ast.h"

#include <string.h>

#include "../../Error/error.h"
#include "Symbols/procedure_symbol.h"
#include "Symbols/module_symbol.h"
#include "Symbols/type_symbol.h"
// ---------------------------------------------------------------------------------------------------------------------
// Universal recursive symbol destructor
// ---------------------------------------------------------------------------------------------------------------------
void BD_SYMBOL_Unload(symbol_t *me) {
    if (me == NULL) return; // nothing to do

    switch (me->type) {
        case MODULE_SYMBOL:
            BD_SYMBOL_LIST_Unload(((module_symbol_t*)me)->lsTypes, true);
            BD_SYMBOL_LIST_Unload(((module_symbol_t*)me)->lsProcedures, true);
        break;

        case TYPE_SYMBOL:
            if (((type_symbol_t*)me)->typeOfType == TYPE_USER_DEFINED)
                BD_SYMBOL_LIST_Unload(((type_symbol_t*)me)->lsFields, true);
        break;

        case PROCEDURE_SYMBOL:
            BD_SYMBOL_LIST_Unload(((procedure_symbol_t*)me)->lsParameters, false);
            BD_SYMBOL_LIST_Unload(((procedure_symbol_t*)me)->lsBuckets, true);
        break;

        default:;
    }

    // finally, unload the buffer itself
    free(me);
}

// ---------------------------------------------------------------------------------------------------------------------
// Universal recursive node destructor
// ---------------------------------------------------------------------------------------------------------------------
void BD_TG_AST_Node_Unload(tg_ast_node_t *me) {
    if (me == NULL) return; // nothing to do

    switch (me->type) {
        default:;
    }

    // finally, unload the buffer itself
    free(me);
}

// ---------------------------------------------------------------------------------------------------------------------
// Node List functions, basically a direct copy of the token list logic
// ---------------------------------------------------------------------------------------------------------------------

tg_ast_node_list_t BD_TG_AST_NODE_LIST_Init() {
    return (tg_ast_node_list_t) {
        NULL, 0, 0
    };
}

void BD_TG_AST_NODE_LIST_Unload(const tg_ast_node_list_t me) {
    for (int i = 0; i < me.length; ++i) {
        BD_TG_AST_Node_Unload(me.nodes[i]);
    }

    free(me.nodes);
}

void BD_TG_AST_NODE_LIST_Add(tg_ast_node_list_t *me, tg_ast_node_t *newNode) {

    // grow the list buffer if needed
    if (!BD_TG_AST_NODE_LIST_grow(me)) {

        // aw man
        ERROR(SUB_BINDER, ERR_INTERNAL, "tight node list cannot grow any larger, out of memory");
        return;
    }

    // when theres sufficient space in the buffer -> add this entry
    me->nodes[me->length] = newNode;
    me->length++;
}

bool BD_TG_AST_NODE_LIST_grow(tg_ast_node_list_t *me) {
    if (me->length < me->capacity) return true; // nothing to do

    // otherwise: expand the capacity and reallocate the buffer
    me->capacity += TG_NODE_LIST_GROWTH;
    tg_ast_node_t **newBuffer = realloc(me->nodes, sizeof(tg_ast_node_t*) * me->capacity);

    // did we manage to allocate a new buffer?
    if (newBuffer == NULL) {

        // if not -> roll back the capacity and return false
        me->capacity -= TG_NODE_LIST_GROWTH;
        return false;
    }

    // otherwise, copy over the new buffer ptr into our list
    me->nodes = newBuffer;
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// Node List List functions, please dont ask, i wouldn't know how to justify this
// ---------------------------------------------------------------------------------------------------------------------
tg_ast_node_list_list_t BD_TG_AST_NODE_LIST_LIST_Init() {
    return (tg_ast_node_list_list_t) {
    NULL, 0, 0
    };
}

void BD_TG_AST_NODE_LIST_LIST_Unload(tg_ast_node_list_list_t me) {
    for (int i = 0; i < me.length; ++i) {
        BD_TG_AST_NODE_LIST_Unload(me.lists[i]);
    }

    free(me.lists);
}

void BD_TG_AST_NODE_LIST_LIST_Add(tg_ast_node_list_list_t *me, tg_ast_node_list_t newList) {

    // grow the list buffer if needed
    if (!BD_TG_AST_NODE_LIST_LIST_grow(me)) {

        // aw man
        ERROR(SUB_BINDER, ERR_INTERNAL, "tight node list list cannot grow any larger, out of memory");
        return;
    }

    // when theres sufficient space in the buffer -> add this entry
    me->lists[me->length] = newList;
    me->length++;
}

bool BD_TG_AST_NODE_LIST_LIST_grow(tg_ast_node_list_list_t *me) {
    if (me->length < me->capacity) return true; // nothing to do

    // otherwise: expand the capacity and reallocate the buffer
    me->capacity += TG_NODE_LIST_GROWTH;
    tg_ast_node_list_t *newBuffer = realloc(me->lists, sizeof(tg_ast_node_list_t) * me->capacity);

    // did we manage to allocate a new buffer?
    if (newBuffer == NULL) {

        // if not -> roll back the capacity and return false
        me->capacity -= TG_NODE_LIST_GROWTH;
        return false;
    }

    // otherwise, copy over the new buffer ptr into our list
    me->lists = newBuffer;
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// Symbol List functions, basically a direct copy of the token list logic
// ---------------------------------------------------------------------------------------------------------------------

symbol_list_t BD_SYMBOL_LIST_Init() {
    return (symbol_list_t) {
        NULL,
        0, 0
    };
}

void BD_SYMBOL_LIST_Unload(symbol_list_t me, bool unloadChildren) {

    if (unloadChildren) {
        for (int i = 0; i < me.length; ++i) {
            BD_SYMBOL_Unload(me.symbols[i]);
        }
    }

    free(me.symbols);
}

void BD_SYMBOL_LIST_Add(symbol_list_t *me, symbol_t *symbol) {

    // grow the list buffer if needed
    if (!BD_SYMBOL_LIST_grow(me)) {

        // damn
        ERROR(SUB_BINDER, ERR_INTERNAL, "symbol list cannot grow any larger, out of memory");
        return;
    }

    // when theres sufficient space -> add this entry
    me->symbols[me->length] = symbol;
    me->length++;
}

void BD_SYMBOL_LIST_RemoveLast(symbol_list_t *me) {
    if (me->length == 0) return;

    me->length--;
    me->symbols[me->length] = NULL;
}

int BD_SYMBOL_LIST_Find(symbol_list_t *me, char *name) {

    // look through the list of symbols
    for (int i = 0; i < me->length; ++i) {

        // check if this symbol matches the name we're looking for
        if (strcmp(me->symbols[i]->name, name) == 0) {
            return i;
        }
    }

    // otherwise: no luck
    return -1;
}

bool BD_SYMBOL_LIST_grow(symbol_list_t *me) {
    if (me->length < me->capacity) return true; // no need to grow the buffer

    // expand the capacity of this buffer
    me->capacity += BD_SYMBOL_LIST_GROWTH;
    symbol_t **newBuffer = realloc(me->symbols, me->capacity * sizeof(symbol_t *));

    // did we manage to allocate a new buffer?
    if (newBuffer == NULL) {

        // if not -> roll back the capacity and return false
        me->capacity -= BD_SYMBOL_LIST_GROWTH;
        return false;
    }

    // if we did -> copy over the new buffer ptr to our list
    me->symbols = newBuffer;
    return true;
}