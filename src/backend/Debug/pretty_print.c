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
            printf("[%000d]: " BYEL "%s " CRESET "(L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].line, tokens.tokens[i].column);
        else
            printf("[%000d]: " BYEL "%s," CYN " '%s'" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, tokens.tokens[i].line, tokens.tokens[i].column);
    }
}
