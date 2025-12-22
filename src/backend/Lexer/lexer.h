//
// Created by ds on 12/21/25.
//

#ifndef LEXER_H
#define LEXER_H
#include "source.h"
#include "token.h"

// ---------------------------------------------------------------------------------------------------------------------
// Our lexer instance struct
// (because i am an object oriented cave man, ooga booga)
typedef struct LEXER {
    // The source we are operating on
    source_t source;

    // the current index we're looking at
    uint64_t pos;

    // our list of lexed tokens
    token_list_t tokens;
} lexer_t;

// our static lexer instance
extern lexer_t *LEXER_Instance;

// methods
void LEXER_Init(source_t source);
void LEXER_Unload();

#endif //LEXER_H
