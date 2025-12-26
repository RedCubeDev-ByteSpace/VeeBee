//
// Created by ds on 12/23/25.
//
#include "pretty_print.h"
#include "ansi.h"

#include <stdio.h>

void DBG_PRETTY_PRINT_Print_TokenList(token_list_t tokens) {
    printf("List of tokens:\n");

    for (int i = 0; i < tokens.length; i++) {
        if (tokens.tokens[i].strValue == NULL)
            printf("[%d]: " BYEL "%s " CRESET "(L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].line, tokens.tokens[i].column);
        else {
            if (tokens.tokens[i].type == TK_LT_STRING) {
                printf("[%d]: " BYEL "%s," CYN " '%s'" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, tokens.tokens[i].line, tokens.tokens[i].column);
            } else if (tokens.tokens[i].type == TK_LT_BOOLEAN) {
                    printf("[%d]: " BYEL "%s," CYN " '%s'" GRN " %d" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, *((bool*)tokens.tokens[i].value), tokens.tokens[i].line, tokens.tokens[i].column);
            } else {
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
        }
    }
}
