//
// Created by ds on 12/23/25.
//

#ifndef PRETTY_PRINT_H
#define PRETTY_PRINT_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"
#include "Debug/ansi.h"

void DBG_PRETTY_PRINT_Print_TokenList(token_list_t tokens);
void DBG_PRETTY_PRINT_Print_TokenList_AsSource(token_list_t tokens);
void DBG_PRETTY_PRINT_Print_LSAstNode(ls_ast_node_t *me, int indent);
void DBG_PRETTY_PRINT_Print_LSAstNode_List(ls_ast_node_list_t me, int indent);

#define INDENT(NUM)                     \
    for (int i_i = 0; i_i < NUM; i_i++) \
        printf(" ");                    \

#define NODE(IDENT)         \
    case IDENT:             \
      printf(CRESET "[" BCYN "%s" CRESET "]\n", #IDENT); \

#define FIELD(FIELD)          \
    INDENT(indent)            \
    printf("   ↳ " BGRN FIELD CRESET ": "); \

#define VALUE(TOKEN)                       \
    if (TOKEN == NULL)                     \
        printf("NULL\n");                  \
    else                                   \
        printf(BYEL "'%s'\n" CRESET, TOKEN->strValue); \

#define VALUE_SET(TOKEN)           \
    if (TOKEN == NULL)             \
        printf("-\n");             \
    else                           \
        printf(BGRN "o\n" CRESET); \

#define SUBNODE(NODE)     \
    if (NODE == NULL)     \
        printf("NULL\n"); \
    else {                \
        printf("\n");     \
        DBG_PRETTY_PRINT_Print_LSAstNode((ls_ast_node_t*)(NODE), indent + 4); \
    } \


#define SUBNODES(NODE_LIST) \
    printf("(%d)\n", NODE_LIST.length);           \
    DBG_PRETTY_PRINT_Print_LSAstNode_List(NODE_LIST, indent + 4); \


#endif //PRETTY_PRINT_H
