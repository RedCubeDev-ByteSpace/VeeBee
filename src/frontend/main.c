#include <stdio.h>
#include "../backend/veebee.h"
#include "Debug/pretty_print.h"
#include "Error/error.h"
#include "Lexer/lexer.h"
#include "Lexer/source.h"

int main(void) {
    source_t source = SOURCE_Init_FromFile("test.bee");
    lexer_t *lexer = LEXER_Init(source);
    LEXER_Lex(lexer);

    DBG_PRETTY_PRINT_Print_TokenList(lexer->tokens);
    DBG_PRETTY_PRINT_Print_TokenList_AsSource(lexer->tokens);

    LX_TOKEN_LIST_Unload(lexer->tokens);
    LEXER_Unload(lexer);
    SOURCE_Unload(source);

    return 0;
}
