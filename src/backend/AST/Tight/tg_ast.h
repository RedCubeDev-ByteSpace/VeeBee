//
// Created by ds on 1/28/26.
//

#ifndef TG_AST_H
#define TG_AST_H

#define TG_STATEMENT 0b0000'0001
#define TG_EXPRESSION 0b0000'0010
#include "Lexer/source.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// TIGHT AST
// ---------------------------------------------------------------------------------------------------------------------

typedef enum TIGHT_NODE_TYPE {

    // Statements
    TG_REDIM_STATEMENT,
    TG_ASSIGNMENT_STATEMENT,
    TG_ARRAY_ASSIGNMENT_STATEMENT,
    TG_IF_STATEMENT,
    TG_SELECT_STATEMENT,
    TG_FOR_STATEMENT,
    TG_DO_STATEMENT,
    TG_WHILE_STATEMENT,
    TG_GOTO_STATEMENT,
    TG_LABEL_STATEMENT,
    TG_EXIT_STATEMENT,
    TG_CALL_STATEMENT,

    // Expressions
    TG_LITERAL_EXPRESSION,
    TG_UNARY_EXPRESSION,
    TG_BINARY_EXPRESSION,
    TG_NAME_REFERENCE_EXPRESSION,
    TG_CALL_REFERENCE_EXPRESSION,

} tg_node_type_t;

// ---------------------------------------------------------------------------------------------------------------------
// the basis for all our tight AST nodes
typedef struct TIGHT_AST_NODE {
    tg_node_type_t type;
} tg_ast_node_t;
void BD_TG_AST_Node_Unload(tg_ast_node_t *me);

// ---------------------------------------------------------------------------------------------------------------------
// collection of tight AST nodes
#define TG_NODE_LIST_GROWTH 8
typedef struct TIGHT_AST_NODE_LIST {
    tg_ast_node_t **nodes;
    uint32_t length;
    uint32_t capacity;
} tg_ast_node_list_t;

tg_ast_node_list_t BD_TG_AST_NODE_LIST_Init();
void BD_TG_AST_NODE_LIST_Unload(tg_ast_node_list_t me);
void BD_TG_AST_NODE_LIST_Add(tg_ast_node_list_t *me, tg_ast_node_t *newNode);
bool BD_TG_AST_NODE_LIST_grow(tg_ast_node_list_t *me);

// ---------------------------------------------------------------------------------------------------------------------
// SYMBOLS
// ---------------------------------------------------------------------------------------------------------------------

typedef enum SYMBOL_TYPE {

    // Absolutely no idea what this symbol is
    // -> figure this out at runtime
    UNKNOWN_SYMBOL,

    // This is a module
    MODULE_SYMBOL,

    // This is a procedure of some kind
    PROCEDURE_SYMBOL,

    // This is a data type
    TYPE_SYMBOL,

    // This is a variable of some kind
    PARAMETER_SYMBOL,      // a parameter for this procedure
    LOCAL_VARIABLE_SYMBOL, // a variable local to this procedure
    TYPE_FIELD_SYMBOL,     // a field of a user defined field

} symbol_type_t;

// ---------------------------------------------------------------------------------------------------------------------
// the basis for all our symbols
typedef struct SYMBOL {
    symbol_type_t type;
    char name[MAX_IDENTIFIER_LENGTH + 1];
    span_t declarationSpan;
} symbol_t;

static const char *BD_DEFAULT_MODULE_NAME = "Default";
void BD_SYMBOL_Unload(symbol_t *me);

// ---------------------------------------------------------------------------------------------------------------------
// a dynamically sized list of symbols
#define BD_SYMBOL_LIST_GROWTH 8
typedef struct SYMBOL_LIST {
    symbol_t **symbols;
    uint32_t length;
    uint32_t capacity;
} symbol_list_t;

symbol_list_t BD_SYMBOL_LIST_Init();
void BD_SYMBOL_LIST_Unload(symbol_list_t me, bool unloadChildren);
void BD_SYMBOL_LIST_Add(symbol_list_t *me, symbol_t *symbol);
int BD_SYMBOL_LIST_Find(symbol_list_t *me, char *name);
bool BD_SYMBOL_LIST_grow(symbol_list_t *me);

#endif //TG_AST_H
