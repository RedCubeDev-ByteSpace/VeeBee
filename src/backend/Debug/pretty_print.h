//
// Created by ds on 12/23/25.
//

#ifndef PRETTY_PRINT_H
#define PRETTY_PRINT_H
#include <stddef.h>
#include <stdio.h>
#include <wchar.h>
#include "AST/Loose/ls_ast.h"
#include "Lexer/token.h"
#include "Debug/ansi.h"

void DBG_PRETTY_PRINT_Print_TokenList(token_list_t tokens);
void DBG_PRETTY_PRINT_Print_TokenList_AsSource(token_list_t tokens);
void DBG_PRETTY_PRINT_Print_LSAstNode(ls_ast_node_t *me, bool finalEntry);
void DBG_PRETTY_PRINT_Print_LSAstNode_List(ls_ast_node_list_t me);

extern char DBG_INDENT_BUFFER[256];
extern int DBG_INDENT_LENGTH;

#define DBG_INIT_INDENT()  \
    DBG_INDENT_LENGTH = 0;  \
    DBG_INDENT_BUFFER[DBG_INDENT_LENGTH] = 0; \

#define DBG_ADD_LINE_INDENT()                  \
DBG_INDENT_BUFFER[DBG_INDENT_LENGTH++] = 0xE2; \
DBG_INDENT_BUFFER[DBG_INDENT_LENGTH++] = 0x94; \
DBG_INDENT_BUFFER[DBG_INDENT_LENGTH++] = 0x82; \
DBG_INDENT_BUFFER[DBG_INDENT_LENGTH++] = ' ';  \
DBG_INDENT_BUFFER[DBG_INDENT_LENGTH++] = ' ';  \
DBG_INDENT_BUFFER[DBG_INDENT_LENGTH] = 0; \

#define DBG_ADD_EMPTY_INDENT()                 \
DBG_INDENT_BUFFER[DBG_INDENT_LENGTH++] = ' ';  \
DBG_INDENT_BUFFER[DBG_INDENT_LENGTH++] = ' ';  \
DBG_INDENT_BUFFER[DBG_INDENT_LENGTH++] = ' ';  \
DBG_INDENT_BUFFER[DBG_INDENT_LENGTH] = 0; \

#define DBG_REMOVE_INDENT() \
    DBG_INDENT_LENGTH -= 3; \
    if (DBG_INDENT_BUFFER[DBG_INDENT_LENGTH] != ' ') \
        DBG_INDENT_LENGTH -= 2; \
    DBG_INDENT_BUFFER[DBG_INDENT_LENGTH] = 0; \

#define INDENT()                     \
    printf(GRY "%s", DBG_INDENT_BUFFER); \

#define NODE(IDENT)         \
    case IDENT:             \
        if (finalEntry) { \
            printf(GRY "└─" CRESET "[" BCYN "%s" CRESET "]\n", #IDENT); \
            DBG_ADD_EMPTY_INDENT() \
        } else { \
            printf(GRY "├─" CRESET "[" BCYN "%s" CRESET "]\n", #IDENT); \
            DBG_ADD_LINE_INDENT() \
        } \

#define END_NODE() \
    DBG_REMOVE_INDENT() \
    break;         \

#define FIELD(FIELD)                        \
    INDENT()                                \
    printf(GRY "├─" CRESET "<" BGRN FIELD CRESET ">: "); \

#define FIELD_FINAL(FIELD)                        \
    INDENT()                            \
    printf(GRY "└─" CRESET "<" BGRN FIELD CRESET ">: "); \

#define VALUE(TOKEN)                                   \
    if (TOKEN == NULL)                                 \
        printf("NULL\n");                              \
    else                                               \
        printf(BYEL "'%s'\n" CRESET, TOKEN->strValue); \

#define VALUE_INT(TOKEN)                               \
    if (TOKEN == NULL)                                 \
        printf("NULL\n");                              \
    else                                               \
        printf(BYEL "'%s'" BCYN " (%d)\n" CRESET, TOKEN->strValue, *((int16_t*)TOKEN->value)); \

#define VALUE_SET(TOKEN)           \
    if (TOKEN == NULL)             \
        printf(BRED "x\n" CRESET);     \
    else                           \
        printf(BGRN "o\n" CRESET);    \

#define SUBNODE(NODE, FINAL)                                             \
    if (NODE == NULL)                                             \
        printf(BLU "NULL\n");                                     \
    else {                                                        \
        printf("\n");                                             \
        if (FINAL) {                                              \
            DBG_ADD_EMPTY_INDENT()                                \
        } else {                                                  \
            DBG_ADD_LINE_INDENT()                                 \
        }                                                         \
        DBG_PRETTY_PRINT_Print_LSAstNode((ls_ast_node_t*)(NODE), true); \
        DBG_REMOVE_INDENT()                                       \
    }                                                             \


#define SUBNODES(NODE_LIST, FINAL)                            \
    printf("(%d)\n", NODE_LIST.length);                       \
    if (FINAL) {                                              \
        DBG_ADD_EMPTY_INDENT()                                \
    } else {                                                  \
        DBG_ADD_LINE_INDENT()                                 \
    }                                                         \
    DBG_PRETTY_PRINT_Print_LSAstNode_List(NODE_LIST);         \
    DBG_REMOVE_INDENT()                                       \


#endif //PRETTY_PRINT_H
