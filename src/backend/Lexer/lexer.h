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

    // the current line number and column
    uint64_t line;
    uint64_t column;

    // our list of lexed tokens
    token_list_t tokens;
} lexer_t;

// ---------------------------------------------------------------------------------------------------------------------
// Public methods
lexer_t *LEXER_Init(source_t source);
void LEXER_Lex(lexer_t *me);
void LEXER_Unload(lexer_t *me);

// ---------------------------------------------------------------------------------------------------------------------
// Private methods
void LEXER_lexWhitespace(lexer_t *me);
void LEXER_lexWord(lexer_t *me);
void LEXER_lexClassifyWord(lexer_t *me, token_t *token);
void LEXER_lexString(lexer_t *me);
void LEXER_lexNumber(lexer_t *me);
void LEXER_lexComment(lexer_t *me);
token_t LEXER_createToken(lexer_t *me, token_type_t type, uint64_t start, uint64_t end);
void LEXER_copyTokenValue(lexer_t *me, token_t *token, uint64_t start, uint64_t end);


// ---------------------------------------------------------------------------------------------------------------------
// Lil helpers
#define AT_EOF() (me->pos >= me->source.length)
#define CURRENT() (me->source.buffer[me->pos])
#define PEEK(AMOUNT) (me->pos + AMOUNT < me->source.length ? me->source.buffer[me->pos + AMOUNT] : 0)

#define STEP()      \
    me->pos++;      \
    me->column++;   \

#define NEWLINE()   \
    me->line++;     \
    me->column = 0; \

#define PREV_TOKEN()                              \
    (me->tokens.length > 0                        \
     ? (me->tokens.tokens[me->tokens.length - 1]) \
     : (token_t){0})                              \

#endif //LEXER_H
