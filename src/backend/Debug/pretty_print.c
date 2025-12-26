//
// Created by ds on 12/23/25.
//
#include "pretty_print.h"
#include "ansi.h"

#include <stdio.h>

void DBG_PRETTY_PRINT_Print_TokenList(token_list_t tokens) {
    printf("List of tokens:\n");

    for (int i = 0; i < tokens.length; i++) {
        if (tokens.tokens[i].type == TK_LT_STRING) {
            printf("[%d]: " BYEL "%s," CYN " '%s'" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, tokens.tokens[i].line, tokens.tokens[i].column);
        } else if (tokens.tokens[i].type == TK_LT_BOOLEAN) {
            printf("[%d]: " BYEL "%s," CYN " '%s'" GRN " %d" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, *((bool*)tokens.tokens[i].value), tokens.tokens[i].line, tokens.tokens[i].column);
        } else if (tokens.tokens[i].type == TK_LT_NUMBER) {
            if (tokens.tokens[i].numberValueType == NUMBER_VALUE_BYTE)
                printf("[%d]: " BYEL "%s," CYN " '%s'" GRN " NUMBER_VALUE_BYTE %u" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, *((uint8_t*)tokens.tokens[i].value), tokens.tokens[i].line, tokens.tokens[i].column);
            else if (tokens.tokens[i].numberValueType == NUMBER_VALUE_INT)
                printf("[%d]: " BYEL "%s," CYN " '%s'" GRN " NUMBER_VALUE_INT %d" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, *((int16_t*)tokens.tokens[i].value), tokens.tokens[i].line, tokens.tokens[i].column);
            else if (tokens.tokens[i].numberValueType == NUMBER_VALUE_LONG)
                printf("[%d]: " BYEL "%s," CYN " '%s'" GRN " NUMBER_VALUE_LONG %d" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, *((int32_t*)tokens.tokens[i].value), tokens.tokens[i].line, tokens.tokens[i].column);
            else if (tokens.tokens[i].numberValueType == NUMBER_VALUE_LONG_LONG)
                printf("[%d]: " BYEL "%s," CYN " '%s'" GRN " NUMBER_VALUE_LONG_LONG %ld" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, *((int64_t*)tokens.tokens[i].value), tokens.tokens[i].line, tokens.tokens[i].column);
            else if (tokens.tokens[i].numberValueType == NUMBER_VALUE_SINGLE)
                printf("[%d]: " BYEL "%s," CYN " '%s'" GRN " NUMBER_VALUE_SINGLE %f" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, *((float*)tokens.tokens[i].value), tokens.tokens[i].line, tokens.tokens[i].column);
            else if (tokens.tokens[i].numberValueType == NUMBER_VALUE_DOUBLE)
                printf("[%d]: " BYEL "%s," CYN " '%s'" GRN " NUMBER_VALUE_DOUBLE %lf" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, *((double*)tokens.tokens[i].value), tokens.tokens[i].line, tokens.tokens[i].column);
            else if (tokens.tokens[i].numberValueType == NUMBER_VALUE_CURRENCY)
                printf("[%d]: " BYEL "%s," CYN " '%s'" GRN " NUMBER_VALUE_CURRENCY %ld" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, *((int64_t*)tokens.tokens[i].value), tokens.tokens[i].line, tokens.tokens[i].column);

        }
        else {
            printf("[%d]: " BYEL "%s " CRESET "(L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].line, tokens.tokens[i].column);
        }
    }
}

void DBG_PRETTY_PRINT_Print_TokenList_AsSource(token_list_t tokens) {
    int indent = 0;
    printf("Highlighted source reconstructed from the token list:\n");

    for (int i = 0; i < tokens.length; ++i) {
        token_t token = tokens.tokens[i];

        if (token.type == TK_KW_SUB) indent += 2;
        if (token.type == TK_KW_FUNCTION) indent += 2;
        if (token.type == TK_KW_END) indent -= 4;
        if (token.type == TK_KW_EXIT) indent -= 2;
        if (token.type == TK_KW_FOR) indent += 2;
        if (token.type == TK_KW_NEXT) indent -= 2;

        if (token.type == TK_KW_END || token.type == TK_KW_NEXT) {
            printf("\r");
            for (int j = 0; j < indent; ++j) {
                printf(" ");
            }
        }

        if (token.type == TK_EOS) {
            printf("\n");
            for (int j = 0; j < indent; ++j) {
                printf(" ");
            }
            continue;
        }

        if (token.type == TK_ERROR) {
            printf(REDB "<error>" CRESET " ");
            continue;
        }

        if (TOKEN_TYPE_STRING[token.type][3] == 'K' && TOKEN_TYPE_STRING[token.type][4] == 'W') {
            printf(BRED "%s ", token.strValue);
            continue;
        }

        if (token.type == TK_IDENTIFIER) {
            printf(YEL "%s ", token.strValue);
            continue;
        }

        if (token.type == TK_LT_STRING) {
            printf(GRN "\"%s\" ", token.strValue);
            continue;
        }

        if (token.type == TK_LT_BOOLEAN) {
            printf(CYN "%s ", token.strValue);
            continue;
        }

        if (token.type == TK_LT_NUMBER) {
            printf(CYN "%s ", token.strValue);
            continue;
        }

        if (token.type == TK_PC_OPEN_PARENTHESIS) {
            printf(CRESET "(");
            continue;
        }

        if (token.type == TK_PC_CLOSED_PARENTHESIS) {
            printf(CRESET ") ");
            continue;
        }

        if (token.type == TK_PC_COMMA) {
            printf(CRESET ", ");
            continue;
        }

        if (token.type == TK_OP_PLUS) {
            printf(CRESET "+ ");
            continue;
        }

        if (token.type == TK_OP_MINUS) {
            printf(CRESET "- ");
            continue;
        }

        if (token.type == TK_OP_STAR) {
            printf(CRESET "* ");
            continue;
        }

        if (token.type == TK_OP_SLASH) {
            printf(CRESET "/ ");
            continue;
        }

        if (token.type == TK_OP_AND) {
            printf(CRESET "& ");
            continue;
        }

        if (token.type == TK_OP_EQUALS) {
            printf(CRESET "= ");
            continue;
        }
    }
}