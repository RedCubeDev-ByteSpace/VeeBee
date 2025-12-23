//
// Created by ds on 12/23/25.
//
#include <stdlib.h>
#include "lexer.h"

#include <ctype.h>
#include <string.h>

#include "Error/error.h"

// =====================================================================================================================
// Public methods

// ---------------------------------------------------------------------------------------------------------------------
// LEXER_Init:
// lexer object constructor
lexer_t *LEXER_Init(source_t source) {
    lexer_t *lexer = malloc(sizeof(lexer_t));

    // initialize our lexer object
    lexer->source = source;
    lexer->pos  = 0;
    lexer->line = 1;
    lexer->column = 0;
    lexer->tokens = LX_TOKEN_LIST_Init();

    return lexer;
}

// ---------------------------------------------------------------------------------------------------------------------
// LEXER_Unload:
// lexer object destructor
void LEXER_Unload(lexer_t *me) {
    free(me);
}

// ---------------------------------------------------------------------------------------------------------------------
// LEXER_Lex:
// Main lexing function
void LEXER_Lex(lexer_t *me) {
    // make sure we have a lexable source
    if (me->source.type == SC_EMPTY) return;
    if (me->source.length == 0) return;

    // otherwise: step through the file
    while (!AT_EOF()) {
        char current = CURRENT();

        // is this a newline?
        // if so -> insert an END OF STATEMENT
        if (current == '\n') {

            // only add an EOS if we didnt already add one
            if (PREV_TOKEN().type != TK_EOS) {
                LX_TOKEN_LIST_Add(&me->tokens,
                    LEXER_createToken(me, TK_EOS, me->pos, me->pos+1)
                );
            }

            STEP()
            NEWLINE()
            continue;
        }

        // is this a random underscore?
        // if its followed by a newline it allowes the current statement to continue on the next line
        if (current == '_' && PEEK(1) == '\n') {
            // just step over them
            STEP()
            STEP()
            NEWLINE()
            continue;
        }

        // grrrr (also support \r\n)
        if (current == '_' && PEEK(1) == '\r' && PEEK(2) == '\n') {
            STEP()
            STEP()
            STEP()
            NEWLINE()
            continue;
        }

        // is this whitespace?
        if (isspace(current)) {
            LEXER_lexWhitespace(me);
            continue;
        }

        // is this a comment?
        if (current == '\'') {
            LEXER_lexComment(me);
            continue;
        }

        // is this a keyword or identifier?
        if (isalpha(current)) {
            LEXER_lexWord(me);
            continue;
        }

        // is this a string?
        if (current == '"') {
            LEXER_lexString(me);
            continue;
        }

        // is this a number?
        if (isdigit(current)) {
            LEXER_lexNumber(me);
            continue;
        }

        // -------------------------------------------------------------------------------------------------------------
        // punctuation stuffs
        // ------------------
        bool ok = false;
        for (int i = 0; i < NUM_PUNC_AND_OPS; ++i) {
            if (current == PUNCTUATION_AND_OPERATORS[i]) {
                LX_TOKEN_LIST_Add(&me->tokens, LEXER_createToken(me, TK_PC_OPEN_PARENTHESIS + i, me->pos, me->pos+1));
                STEP()

                ok = true;
                break;
            }
        }

        // we found some punctuation
        if (ok) continue;

        // if its nothing we know -> just step over it
        ERROR_AT(SUB_LEXER, ERR_LX_UNEXPECTED_CHARACTER, me->source, SPAN_FromPos(me->line, me->column), "The lexer encountered an unexpected character")
        STEP()
    }

    // if we didnt end with a new line -> add an EOS at the end of the file
    if (PREV_TOKEN().type != TK_EOS) {
        LX_TOKEN_LIST_Add(&me->tokens,
            LEXER_createToken(me, TK_EOS, me->pos, me->pos+1)
        );
    }
}

// =====================================================================================================================
// Private methods

// ---------------------------------------------------------------------------------------------------------------------
// LEXER_lexWhitespace:
// consume whitespace characters until we hit a non whitespace
void LEXER_lexWhitespace(lexer_t *me) {
    // consume any whitespaces we can find
    while (!AT_EOF() && isspace(CURRENT()) && CURRENT() != '\n') {
        STEP()
    }

    // thats all
}

// ---------------------------------------------------------------------------------------------------------------------
// LEXER_lexWord:
// consume alphanumeric characters and underscores until we hit a different character,
// then find out if this is an identifier or a keyword
void LEXER_lexWord(lexer_t *me) {
    uint64_t wordStart = me->pos;
    uint64_t wordLength = 0;

    while (!AT_EOF() && (
        isalnum(CURRENT()) || // alphanumeric characters are allowed
        CURRENT() == '_'      // so are underscores
    )) {
        wordLength++;
        STEP()
    }

    // create a new token for our word
    token_t token = LEXER_createToken(me, TK_IDENTIFIER, wordStart, me->pos);

    // is the identifier length maximum being violated?
    if (wordLength > MAX_IDENTIFIER_LENGTH) {
        wordLength = MAX_IDENTIFIER_LENGTH;
        ERROR_AT(SUB_LEXER, ERR_LX_IDENTIFIER_TOO_LONG, me->source, SPAN_FromToken(token), "The length of an identifier is too long, it will be truncated to 255 characters")
    }

    // copy over the buffer
    LEXER_copyTokenValue(me, &token, wordStart, wordStart + wordLength);

    // turn all characters into their lowercase versions, this discards capitalisation
    for (int i = 0; i < wordLength; ++i) {
        token.strValue[i] = (char)tolower(token.strValue[i]);
    }

    // check if this is a reserved keyword or an identifier
    LEXER_lexClassifyWord(me, &token);

    // add it to the list
    LX_TOKEN_LIST_Add(&me->tokens, token);
}

// ---------------------------------------------------------------------------------------------------------------------
// LEXER_lexClassifyWord:
// find out if this token is a reserved language keyword or an identifier
void LEXER_lexClassifyWord(lexer_t *me, token_t *token) {

    // look through all known keywords
    for (int i = 0; i < NUM_KEYWORDS; ++i) {

        // is this a match?
        if (strcmp(token->strValue, KEYWORDS[i]) == 0) {
            token->type = TK_KW_BEGIN + i;
            return;
        }
    }

    // if we didnt find anything -> definitely an identifier
    token->type = TK_IDENTIFIER;
}

// ---------------------------------------------------------------------------------------------------------------------
// LEXER_lexString:
// collect chars until we find a closing quotation mark
void LEXER_lexString(lexer_t *me) {
    uint64_t tokenStart = me->pos;

    // step over the opening quotation mark
    STEP()

    // set up a buffer for this
    char stringBuffer[MAX_TOKEN_LENGTH + 1];
    uint32_t stringLength = 0;

    // step through the buffer until we hit the end of file or
    while (CURRENT() != '"') {

        // strings are not allowed to be multiline!
        if (AT_EOF() || CURRENT() == '\r' || CURRENT() == '\n') {
            ERROR_AT(SUB_LEXER, ERR_LX_UNTERMINATED_STRING, me->source, SPAN_FromPos(me->line, me->column), "A given string is never terminated")
            break;
        }

        // copy the current char into the string content buffer
        stringBuffer[stringLength] = CURRENT();
        stringLength++;

        // next char!
        STEP()

        // check if we have encountered an escaped quotation mark
        if (CURRENT() == '"' && PEEK(1) == '"') {

            // step over them
            STEP()
            STEP()

            // add a single quotation mark onto our string buffer
            stringBuffer[stringLength] = '"';
            stringLength++;
        }
    }

    // step over the closing quotation mark
    STEP()

    // create a new token
    token_t token = LEXER_createToken(me, TK_LT_STRING, tokenStart, me->pos);

    // allocate a buffer for this string and copy it over
    token.strValue = malloc(stringLength + 1);
    memcpy(token.strValue, stringBuffer, stringLength);

    // terminate this absolute fool
    token.strValue[stringLength] = 0;

    LX_TOKEN_LIST_Add(&me->tokens, token);
}

// ---------------------------------------------------------------------------------------------------------------------
// LEXER_lexNumber:
// read in as many numeric chars or decimal points as we can find
void LEXER_lexNumber(lexer_t *me) {
    uint64_t numberStart = me->pos;
    uint64_t numberLength = 0;

    // TODO: parse numbers into their actual binary representation
    // TODO: support hex and binary literals

    while (!AT_EOF() && (
        isdigit(CURRENT()) || // numeric characters are allowed
        CURRENT() == '.'      // so are decimal points
    )) {
        numberLength++;
        STEP()
    }

    // create a new token for our word
    token_t token = LEXER_createToken(me, TK_LT_NUMBER, numberStart, numberStart + numberLength);

    // copy over the buffer
    LEXER_copyTokenValue(me, &token, numberStart, numberStart + numberLength);

    // add it to the list
    LX_TOKEN_LIST_Add(&me->tokens, token);
}

// ---------------------------------------------------------------------------------------------------------------------
// LEXER_lexComment:
// step through our source until we hit a new line or the end of the file
void LEXER_lexComment(lexer_t *me) {
    while (!AT_EOF() && CURRENT() != '\n') {
        STEP()
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// LEXER_createToken:
// helper function, creates a new token object with correctly set locations
token_t LEXER_createToken(lexer_t *me, token_type_t type, uint64_t start, uint64_t end) {
    token_t token = { 0 };

    // set the token type
    token.type = type;

    // set starting location and length
    token.srcPos = start;
    token.length = end - start;

    // set source code location
    token.line = me->line;
    token.column = me->column - token.length;

    // mark this token as not having a value for now
    token.strValue = NULL;

    return token;
}

// ---------------------------------------------------------------------------------------------------------------------
// LEXER_createToken:
// copy part of the input buffer into this token, used for identifiers and literals
void LEXER_copyTokenValue(lexer_t *me, token_t *token, uint64_t start, uint64_t end) {
    uint64_t len = end - start;

    // make sure this doesnt violate our max allowed token length!
    if (len > MAX_TOKEN_LENGTH) {
        ERROR_AT(SUB_LEXER, ERR_LX_TOKEN_VALUE_TOO_LONG, me->source, SPAN_FromToken(*token), "The value of the given token is larger than the allowed maximum, it will be truncated")
        len = MAX_TOKEN_LENGTH;
    }

    // copy over the segment from the source buffer into the tokens buffer
    token->strValue = malloc(len + 1);
    memcpy(token->strValue, me->source.buffer + start, len);

    // null terminate this string
    token->strValue[len] = 0;
}