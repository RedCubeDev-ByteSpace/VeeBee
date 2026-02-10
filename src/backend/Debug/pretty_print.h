//
// Created by ds on 12/23/25.
//

#ifndef PRETTY_PRINT_H
#define PRETTY_PRINT_H
#include <stddef.h>
#include <stdio.h>
#include <wchar.h>
#include "AST/Loose/ls_ast.h"
#include "AST/Tight/program_unit.h"
#include "Lexer/token.h"
#include "Debug/ansi.h"

void DBG_PRETTY_PRINT_Print_TokenList(token_list_t tokens);
void DBG_PRETTY_PRINT_Print_TokenList_AsSource(token_list_t tokens);
void DBG_PRETTY_PRINT_Print_LSAstNode(ls_ast_node_t *me, bool finalEntry);
void DBG_PRETTY_PRINT_Print_LSAstNode_List(ls_ast_node_list_t me);
void DBG_PRETTY_PRINT_Print_ProgramUnit(program_unit_t *me);
void DBG_PRETTY_PRINT_Print_Symbol(symbol_t *me, bool finalEntry);
void DBG_PRETTY_PRINT_Print_SymbolList(symbol_list_t me);
void DBG_PRETTY_PRINT_Print_TGAstNode(tg_ast_node_t *me, bool finalEntry);
void DBG_PRETTY_PRINT_Print_TGAstNode_List(tg_ast_node_list_t me);
void DBG_PRETTY_PRINT_Print_TGAstNode_List_List(tg_ast_node_list_list_t me);
void DBG_PRETTY_PRINT_Print_TGAstNode_List_Buffer(tg_ast_node_list_t *me, uint32_t length);

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

#define LIST(IDENT, FINAL)  \
    if (FINAL) { \
        printf(GRY "└─" CRESET "[" BCYN "%d" CRESET "]\n", IDENT); \
        DBG_ADD_EMPTY_INDENT() \
    } else { \
        printf(GRY "├─" CRESET "[" BCYN "%d" CRESET "]\n", IDENT); \
        DBG_ADD_LINE_INDENT() \
    } \

#define NULL_NODE()   \
    if (finalEntry) { \
        printf(GRY "└─" CRESET "[" BCYN "NULL" CRESET "]\n"); \
    } else { \
        printf(GRY "├─" CRESET "[" BCYN "NULL" CRESET "]\n"); \
    } \

#define END_NODE() \
    DBG_REMOVE_INDENT() \
    break;         \

#define END_LIST() \
    DBG_REMOVE_INDENT() \

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

#define VALUE_SYM(STR)                      \
        printf(BYEL "'%s'\n" CRESET, #STR); \

#define VALUE_STR(STR)                   \
    printf(BYEL "'%s'\n" CRESET, (STR)); \

#define VALUE_NUM(NUM)                 \
    printf(BCYN "'%d' (0x%x)\n" CRESET, NUM, NUM); \

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

#define TG_SUBNODE(NODE, FINAL)                                   \
    if (NODE == NULL)                                             \
        printf(BLU "NULL\n");                                     \
    else {                                                        \
        printf("\n");                                             \
        if (FINAL) {                                              \
            DBG_ADD_EMPTY_INDENT()                                \
        } else {                                                  \
            DBG_ADD_LINE_INDENT()                                 \
        }                                                         \
        DBG_PRETTY_PRINT_Print_TGAstNode((tg_ast_node_t*)(NODE), true); \
        DBG_REMOVE_INDENT()                                       \
    }

#define SUBNODES(NODE_LIST, FINAL)                            \
    printf("(%d)\n", NODE_LIST.length);                       \
    if (FINAL) {                                              \
        DBG_ADD_EMPTY_INDENT()                                \
    } else {                                                  \
        DBG_ADD_LINE_INDENT()                                 \
    }                                                         \
    DBG_PRETTY_PRINT_Print_LSAstNode_List(NODE_LIST);         \
    DBG_REMOVE_INDENT()                                       \

#define TG_SUBNODES(NODE_LIST, FINAL)                         \
    printf("(%d)\n", NODE_LIST.length);                       \
    if (FINAL) {                                              \
        DBG_ADD_EMPTY_INDENT()                                \
    } else {                                                  \
        DBG_ADD_LINE_INDENT()                                 \
    }                                                         \
    DBG_PRETTY_PRINT_Print_TGAstNode_List(NODE_LIST);         \
    DBG_REMOVE_INDENT()                                       \

#define TG_SUBNODE_LISTS(NODE_LIST, FINAL)                    \
    printf("(%d)\n", NODE_LIST.length);                       \
    if (FINAL) {                                              \
        DBG_ADD_EMPTY_INDENT()                                \
    } else {                                                  \
        DBG_ADD_LINE_INDENT()                                 \
    }                                                         \
    DBG_PRETTY_PRINT_Print_TGAstNode_List_List(NODE_LIST);    \
    DBG_REMOVE_INDENT()                                       \

#define SYMBOL(IDENT)     \
    case IDENT:           \
        if (finalEntry) { \
            printf(GRY "└─" CRESET "[" BCYN "%s" CRESET "]\n", #IDENT); \
             DBG_ADD_EMPTY_INDENT() \
        } else { \
            printf(GRY "├─" CRESET "[" BCYN "%s" CRESET "]\n", #IDENT); \
            DBG_ADD_LINE_INDENT() \
        } \

#define END_SYMBOL() \
    DBG_REMOVE_INDENT() \
    break;         \

#define SUBSYMBOL(SYM, FINAL)                                     \
    if (SYM == NULL)                                              \
        printf(BLU "NULL\n");                                     \
    else {                                                        \
        printf("\n");                                             \
        if (FINAL) {                                              \
            DBG_ADD_EMPTY_INDENT()                                \
        } else {                                                  \
            DBG_ADD_LINE_INDENT()                                 \
        }                                                         \
        DBG_PRETTY_PRINT_Print_Symbol((symbol_t*)(SYM), true);    \
        DBG_REMOVE_INDENT()                                       \
    }                                                             \

#define SUBSYMBOL_STUB(SYM, FINAL)                                \
    if (SYM == NULL)                                              \
        printf(BLU "NULL\n");                                     \
    else {                                                        \
        printf("\n");                                             \
        if (FINAL) {                                              \
            DBG_ADD_EMPTY_INDENT()                                \
        } else {                                                  \
            DBG_ADD_LINE_INDENT()                                 \
        }                                                         \
        DBG_PRETTY_PRINT_Print_Symbol_Stub((symbol_t*)(SYM), true);\
        DBG_REMOVE_INDENT()                                       \
    }                                                             \

#define SUBSYMBOLS(SYM_LIST, FINAL)                           \
    printf("(%d)\n", SYM_LIST.length);                        \
    if (FINAL) {                                              \
        DBG_ADD_EMPTY_INDENT()                                \
    } else {                                                  \
        DBG_ADD_LINE_INDENT()                                 \
    }                                                         \
    DBG_PRETTY_PRINT_Print_SymbolList(SYM_LIST);              \
    DBG_REMOVE_INDENT()                                       \

#endif //PRETTY_PRINT_H
