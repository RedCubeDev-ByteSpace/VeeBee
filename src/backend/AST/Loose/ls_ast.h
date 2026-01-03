//
// Created by ds on 12/29/25.
//

#ifndef AST_H
#define AST_H
#include <stdint.h>

typedef enum LOOSE_NODE_TYPE {
    LS_NONE = 0,

    // Members
    LS_MODULE_MEMBER           = 0x10 | 0b0000'0001,
    LS_FUNCTION_MEMBER         = 0x20 | 0b0000'0001,
    LS_SUBROUTINE_MEMBER       = 0x30 | 0b0000'0001,
    LS_TYPE_MEMBER             = 0x40 | 0b0000'0001,

    // Statements
    LS_IF_STATEMENT            = 0x10 | 0b0000'0010,
    LS_ASSIGNMENT_STATEMENT    = 0x20 | 0b0000'0010,
    LS_SUB_CALL_STATEMENT      = 0x30 | 0b0000'0010,
    LS_FUNCTION_CALL_STATEMENT = 0x30 | 0b0000'0010,
    LS_FOR_STATEMENT           = 0x40 | 0b0000'0010,
    LS_WHILE_STATEMENT         = 0x40 | 0b0000'0010,
    LS_LABEL_STATEMENT         = 0x50 | 0b0000'0010,
    LS_GOTO_STATEMENT          = 0x60 | 0b0000'0010,

    // Expressions
    LS_UNARY_EXPRESSION         = 0x10 | 0b0000'0100,
    LS_BINARY_EXPRESSION        = 0x20 | 0b0000'0100,
    LS_FUNCTION_CALL_EXPRESSION = 0x30 | 0b0000'0100,

    // Clauses
    LS_AS_CLAUSE                = 0x10 | 0b0000'1000,
    LS_TYPE_FIELD_CLAUSE        = 0x20 | 0b0000'1000,
    LS_PARAMETER_CLAUSE         = 0x30 | 0b0000'1000,
} ls_node_type_t;

#define LS_MEMBER 0b0000'0001
#define LS_STATEMENT 0b0000'0010
#define LS_EXPRESSION 0b0000'0100
#define LS_CLAUSE 0b0000'1000

// ---------------------------------------------------------------------------------------------------------------------
// the basis for all out AST nodes

typedef struct LOOSE_AST_NODE {
    ls_node_type_t type;
 } ls_ast_node_t;

void PS_LS_Node_Unload(ls_ast_node_t *me);

// ---------------------------------------------------------------------------------------------------------------------
// collection of nodes
#define PS_NODE_LIST_GROWTH 8
typedef struct LOOSE_AST_NODE_LIST {
    ls_ast_node_t **nodes;
    uint32_t length;
    uint32_t capacity;
} ls_ast_node_list_t;

ls_ast_node_list_t PS_LS_AST_NODE_LIST_Init();
void PS_LS_AST_NODE_LIST_Unload(ls_ast_node_list_t me);
void PS_LS_AST_NODE_LIST_Add(ls_ast_node_list_t *me, ls_ast_node_t *newNode);
bool PS_LS_AST_NODE_LIST_grow(ls_ast_node_list_t *me);

#endif //AST_H
