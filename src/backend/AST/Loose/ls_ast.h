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
    LS_PROPERTY_GET_MEMBER     = 0x40 | 0b0000'0001, // TODO
    LS_PROPERTY_SET_MEMBER     = 0x50 | 0b0000'0001, // TODO
    LS_PROPERTY_LET_MEMBER     = 0x60 | 0b0000'0001, // TODO
    LS_TYPE_MEMBER             = 0x70 | 0b0000'0001,
    LS_ENUM_MEMBER             = 0x80 | 0b0000'0001, // TODO
    LS_FIELD_MEMBER            = 0x90 | 0b0000'0001, // TODO
    LS_CONST_MEMBER            = 0xA0 | 0b0000'0001, // TODO

    // Statements
    LS_DIM_STATEMENT           = 0x010 | 0b0000'0010,
    LS_REDIM_STATEMENT         = 0x020 | 0b0000'0010,
    LS_ASSIGNMENT_STATEMENT    = 0x030 | 0b0000'0010,
    LS_IF_STATEMENT            = 0x040 | 0b0000'0010,
    LS_SELECT_STATEMENT        = 0x050 | 0b0000'0010,
    LS_FOR_STATEMENT           = 0x060 | 0b0000'0010, // TODO
    LS_FOR_EACH_STATEMENT      = 0x070 | 0b0000'0010, // TODO
    LS_DO_STATEMENT            = 0x080 | 0b0000'0010, // TODO
    LS_WHILE_STATEMENT         = 0x090 | 0b0000'0010, // TODO
    LS_GOTO_STATEMENT          = 0x0A0 | 0b0000'0010,
    LS_LABEL_STATEMENT         = 0x0B0 | 0b0000'0010,
    LS_ON_ERROR_STATEMENT      = 0x0C0 | 0b0000'0010, // TODO
    LS_RESUME_STATEMENT        = 0x0D0 | 0b0000'0010, // TODO
    LS_WITH_STATEMENT          = 0x0E0 | 0b0000'0010, // TODO
    LS_EXIT_STATEMENT          = 0x0F0 | 0b0000'0010, // TODO
    LS_EXPRESSION_STATEMENT    = 0x100 | 0b0000'0010,

    // Expressions
    LS_LITERAL_EXPRESSION       = 0x10 | 0b0000'0100,
    LS_UNARY_EXPRESSION         = 0x20 | 0b0000'0100, // TODO
    LS_BINARY_EXPRESSION        = 0x30 | 0b0000'0100, // TODO
    LS_REFERENCE_EXPRESSION     = 0x50 | 0b0000'0100,

    // Clauses
    LS_AS_CLAUSE                = 0x10 | 0b0000'1000,
    LS_ARR_RANGE_CLAUSE         = 0x20 | 0b0000'1000,
    LS_TYPE_FIELD_CLAUSE        = 0x30 | 0b0000'1000,
    LS_PARAMETER_CLAUSE         = 0x40 | 0b0000'1000,
    LS_DIM_FIELD_CLAUSE         = 0x50 | 0b0000'1000,
    LS_CONDITIONAL_CLAUSE       = 0x60 | 0b0000'1000,
    LS_ELSE_CLAUSE              = 0x70 | 0b0000'1000,
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
