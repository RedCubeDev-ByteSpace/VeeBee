//
// Created by ds on 12/23/25.
//
#include <stdlib.h>
#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
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

        // is this a comment? (also allow shebangs)
        if (current == '\'' || (current == '#' && PEEK(1) == '!')) {
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
        if (isdigit(current)
            || (current == '.' && isdigit(PEEK(1))) // lopsided decimal
            || (current == '&' && PEEK(1) == 'H')   // hexadecimal
            || (current == '&' && PEEK(1) == 'O')   // octal
            || (current == '&' && PEEK(1) == 'B')   // binary
            ) {
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

    // always add an end of file at the end of a token list
    LX_TOKEN_LIST_Add(&me->tokens,
        LEXER_createToken(me, TK_EOF, me->pos, me->pos+1)
    );
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
    LEXER_classifyWord(me, &token);

    // add it to the list
    LX_TOKEN_LIST_Add(&me->tokens, token);
}

// ---------------------------------------------------------------------------------------------------------------------
// LEXER_lexClassifyWord:
// find out if this token is a reserved language keyword or an identifier
void LEXER_classifyWord(lexer_t *me, token_t *token) {

    // look through all known keywords
    for (int i = 0; i < NUM_KEYWORDS; ++i) {

        // is this a match?
        if (strcmp(token->strValue, KEYWORDS[i]) == 0) {
            token->type = TK_KW_BEGIN + i;
            return;
        }
    }

    // is this actually a boolean literal?
    if (strcmp(token->strValue, "true") == 0 || strcmp(token->strValue, "false") == 0) {
        token->type = TK_LT_BOOLEAN;

        // allocate a value buffer for this and store the value of this literal
        token->value = malloc(sizeof(bool));
        *(bool*)token->value = strcmp(token->strValue, "true") == 0;

        return;
    }

    // if we didn't find anything -> definitely an identifier
    token->type = TK_IDENTIFIER;
}

// ---------------------------------------------------------------------------------------------------------------------
// LEXER_lexString:
// collect chars until we find a closing quotation mark
void LEXER_lexString(lexer_t *me) {
    uint64_t tokenStart = me->pos;

    // remember the starting position for error handling
    span_t startingPosition = SPAN_FromPos(me->line, me->column);

    // step over the opening quotation mark
    STEP()

    // set up a buffer for this
    char stringBuffer[MAX_TOKEN_LENGTH + 1];
    uint32_t stringLength = 0;

    // declare a flag for if this string is too long for the buffer
    bool errStringTooLong = false;

    // step through the buffer until we hit the end of file or
    while (CURRENT() != '"') {

        // strings are not allowed to be multiline!
        if (AT_EOF() || CURRENT() == '\r' || CURRENT() == '\n') {
            ERROR_AT(SUB_LEXER, ERR_LX_UNTERMINATED_STRING, me->source, startingPosition, "A given string is never terminated")
            break;
        }

        // is there still place in the buffer?
        if (stringLength < MAX_TOKEN_LENGTH) {

            // copy the current char into the string content buffer
            stringBuffer[stringLength] = CURRENT();
            stringLength++;
        }

        // otherwise, set the error flag
        else {
            errStringTooLong = true;
        }


        // next char!
        STEP()

        // check if we have encountered an escaped quotation mark
        if (CURRENT() == '"' && PEEK(1) == '"') {

            // step over them
            STEP()
            STEP()

            // is there still place in the buffer?
            if (stringLength < MAX_TOKEN_LENGTH) {

                // add a single quotation mark onto our string buffer
                stringBuffer[stringLength] = '"';
                stringLength++;
            }

            // otherwise, set the error flag
            else {
                errStringTooLong = true;
            }
        }
    }

    // step over the closing quotation mark
    STEP()

    // create a new token
    token_t token = LEXER_createToken(me, TK_LT_STRING, tokenStart, me->pos);

    // make sure to report this error
    if (errStringTooLong) {
        ERROR_AT(SUB_LEXER, ERR_LX_TOKEN_VALUE_TOO_LONG, me->source, SPAN_FromToken(token), "The given string is larger than the allowed maximum token size")
    }

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

    // remember where we started parsing for error reporting
    span_t startingPosition = SPAN_FromPos(me->line, me->column);
    bool errNumberTooLong = false;

    while (!AT_EOF() && (
        isdigit(CURRENT()) || // numeric characters are allowed
        CURRENT() == '.'   || // decimal points are allowed

        CURRENT() == '&'   || // and signs are allowed for hex, oct and bin literals
        CURRENT() == 'H'   || CURRENT() == 'h' || // hex marker
        CURRENT() == 'O'   || CURRENT() == 'o' || // oct marker
        CURRENT() == 'B'   || CURRENT() == 'b' || // bin marker

        CURRENT() == 'E'   || CURRENT() == 'e' || // exponent marker

        CURRENT() == '~'   || // byte type specifier
        CURRENT() == '%'   || // integer type specifier
        CURRENT() == '^'   || // longlong type specifier
        CURRENT() == '!'   || // single type specifier
        CURRENT() == '#'   || // double type specifier
        CURRENT() == '@'   || // currency type specifier

        // hex digits a...f
        (
            (CURRENT() >= 'a' && CURRENT() <= 'f') ||
            (CURRENT() >= 'A' && CURRENT() <= 'F')
        )
    )) {
        if (numberLength < MAX_TOKEN_LENGTH) {
            numberLength++;
        }
        else {
            errNumberTooLong = true;
        }

        // if theres an exponent marker, also allow + and -
        if (CURRENT() == 'E' || CURRENT() == 'e') {
            if (PEEK(1) == '+' || PEEK(1) == '-') {
                STEP()

                if (numberLength < MAX_TOKEN_LENGTH) {
                    numberLength++;
                }
                else {
                    errNumberTooLong = true;
                }
            }
        }

        STEP()
    }

    // make sure to report an error if the number literal is too long
    if (errNumberTooLong) {
        ERROR_AT(SUB_LEXER, ERR_LX_TOKEN_VALUE_TOO_LONG, me->source, SPAN_Between(startingPosition, SPAN_FromPos(me->line, me->column)), "The given number literal is larger than the allowed maximum token size")
        LX_TOKEN_LIST_Add(&me->tokens, LEXER_createToken(me, TK_ERROR, numberStart, me->pos));
        return;
    }

    // find out what the hell sort of number this is
    number_literal_type_t literalType = LEXER_classifyNumber(me, &me->source.buffer[numberStart], startingPosition, numberLength);

    // is this a valid number?
    if (literalType & NUMBER_LITERAL_ERROR) {

        // nope
        LX_TOKEN_LIST_Add(&me->tokens, LEXER_createToken(me, TK_ERROR, numberStart, me->pos));
        return;
    }

    // otherwise! create a token for it
    token_t token = LEXER_createToken(me, TK_LT_NUMBER, numberStart, me->pos);

    // then, parse the string representation of the number into an actual number
    if (!LEXER_convertIntoNumber(me, &token, literalType, &me->source.buffer[numberStart], numberLength, SPAN_FromToken(token))) {

        // nope
        LX_TOKEN_LIST_Add(&me->tokens, LEXER_createToken(me, TK_ERROR, numberStart, me->pos));
        return;
    }

    // if everything worked out -> were so done here
    LEXER_copyTokenValue(me, &token, numberStart, me->pos);
    LX_TOKEN_LIST_Add(&me->tokens, token);
}

number_literal_type_t LEXER_classifyNumber(lexer_t *me, const char *buffer, span_t startingPosition, uint32_t length) {
    number_literal_lexing_type_t lexingType = NUMBER_LITERAL_ERROR;
    number_literal_value_type_t valueType = NUMBER_VALUE_NONE;

    // check if this is a hex, oct or bin literal (&H, &O, &B)
    if (length >= 2 && buffer[0] == '&') {
        if      (buffer[1] == 'H' || buffer[1] == 'h') lexingType = NUMBER_LITERAL_HEX;
        else if (buffer[1] == 'O' || buffer[1] == 'o') lexingType = NUMBER_LITERAL_OCT;
        else if (buffer[1] == 'B' || buffer[1] == 'b') lexingType = NUMBER_LITERAL_BIN;
        else {
            // only there three are allowed
            ERROR_AT(SUB_LEXER, ERR_LX_UNEXPECTED_LITERAL_QUALIFIER, me->source, SPAN_ShiftByChars(startingPosition, 1), "Unknown literal qualifier, expected 'H' (hex), 'O' (octal), or 'B' (binary)")
            return NUMBER_LITERAL_ERROR | NUMBER_VALUE_NONE;
        }
    }


    // check if there is a type specifier at the end of this literal
    if      (buffer[length - 1] == '~') valueType = NUMBER_VALUE_BYTE;
    else if (buffer[length - 1] == '%') valueType = NUMBER_VALUE_INT;
    else if (buffer[length - 1] == '&') valueType = NUMBER_VALUE_LONG;
    else if (buffer[length - 1] == '^') valueType = NUMBER_VALUE_LONG_LONG;

    else if (buffer[length - 1] == '!') valueType = NUMBER_VALUE_SINGLE;
    else if (buffer[length - 1] == '#') valueType = NUMBER_VALUE_DOUBLE;
    else if (buffer[length - 1] == '@') valueType = NUMBER_VALUE_CURRENCY;

    bool hasTypeSpecifier = valueType != NUMBER_VALUE_NONE;

    // check if this literal is in scientific notation
    if (lexingType == NUMBER_LITERAL_ERROR) {
        for (int i = 0; i < length; ++i) {
            char c = tolower(buffer[i]);
            if (c == 'e') {
                lexingType = NUMBER_LITERAL_SCIENTIFIC;
                break;
            }
        }
    }

    // check if this literal is a decimal
    if (lexingType == NUMBER_LITERAL_ERROR) {
        for (int i = 0; i < length; ++i) {
            char c = buffer[i];
            if (c == '.') {
                lexingType = NUMBER_LITERAL_DECIMAL;
                break;
            }
        }
    }

    // otherwise this is probably a normal integer
    if (lexingType == NUMBER_LITERAL_ERROR)
        lexingType = NUMBER_LITERAL_INT;

    // if we havent specified a value type yet -> assume the defaults
    if (valueType == NUMBER_VALUE_NONE) {
        // whole numbers default to int
        if (lexingType == NUMBER_LITERAL_INT) valueType = NUMBER_VALUE_INT;

        // hex, oct, bin default to long
        if (lexingType == NUMBER_LITERAL_HEX) valueType = NUMBER_VALUE_LONG;
        if (lexingType == NUMBER_LITERAL_OCT) valueType = NUMBER_VALUE_LONG;
        if (lexingType == NUMBER_LITERAL_BIN) valueType = NUMBER_VALUE_LONG;

        // decimals and scientific notation default to double
        if (lexingType == NUMBER_LITERAL_DECIMAL) valueType = NUMBER_VALUE_DOUBLE;
        if (lexingType == NUMBER_LITERAL_SCIENTIFIC) valueType = NUMBER_VALUE_DOUBLE;
    }

    // check if the value type and literal type are incompatible
    if (
        // hex, oct, bin literals are incompatible with decimal types
        (
            (lexingType == NUMBER_LITERAL_HEX || lexingType == NUMBER_LITERAL_OCT || lexingType == NUMBER_LITERAL_BIN)
            &&
            (valueType == NUMBER_VALUE_SINGLE || valueType == NUMBER_VALUE_DOUBLE || valueType == NUMBER_VALUE_CURRENCY)
        )

        ||

        // decimal and scientific notation is incompatible with the integer types
        (
            (lexingType == NUMBER_LITERAL_DECIMAL || lexingType == NUMBER_LITERAL_SCIENTIFIC)
            &&
            (valueType == NUMBER_VALUE_BYTE || valueType == NUMBER_VALUE_INT || valueType == NUMBER_VALUE_LONG || valueType == NUMBER_VALUE_LONG_LONG)
        )

        ||

        // scientific notation is also incompatible with the currency type
        (
            lexingType == NUMBER_LITERAL_SCIENTIFIC && valueType == NUMBER_VALUE_CURRENCY
        )
        ) {
        ERROR_AT(SUB_LEXER, ERR_LX_INCOMPATIBLE_NUMERIC_TYPE_AND_LITERAL, me->source, SPAN_ExtendByChars(startingPosition, length), "The given type of literal is incompatible with the requested numeric data type")
        return NUMBER_LITERAL_ERROR | NUMBER_VALUE_NONE;
    }

    // now, make sure that there are no disallowed characters in our literal
    bool foundDecimalPoint = false; // keep track of if we found a decimal point, a number literal is only allowed to contain one decimal point
    bool foundE = false; // keep track of if we found a an exponentiation marker, again this is only supposed to be in a literal once
    int placesAfterDecimalPoint = 0; // keep track of how many digits we come across after a decimal point, some data types have restrictions on this

    for (int i = 0; i < (hasTypeSpecifier ? length -1 : length); ++i) {
        // underscores are fair game everywhere
        if (buffer[i] == '_') continue;

        switch (lexingType) {

            // this should never happen
            case NUMBER_LITERAL_ERROR:
                break;

            // ---------------------------------------------------------------------------------------------------------
            // normal integers
            case NUMBER_LITERAL_INT:
                // only allow the digits 0...9
                if (isdigit(buffer[i])) continue;

                ERROR_AT(SUB_LEXER, ERR_LX_UNEXPECTED_CHAR_IN_NUMERIC_LITERAL, me->source, SPAN_ShiftByChars(startingPosition, i), "Non digit character in integer literal")
                return NUMBER_LITERAL_ERROR | NUMBER_VALUE_NONE;
            break;

            // ---------------------------------------------------------------------------------------------------------
            // normal decimals
            case NUMBER_LITERAL_DECIMAL:
                // allow digits 0...9
                if (isdigit(buffer[i])) {
                    if (!foundDecimalPoint) continue;

                    // allow at maximum 4 decimal places for literals of type currency
                    placesAfterDecimalPoint++;
                    if (valueType == NUMBER_VALUE_CURRENCY) {
                        if (placesAfterDecimalPoint <= 4) continue;

                        ERROR_AT(SUB_LEXER, ERR_LX_UNEXPECTED_CHAR_IN_NUMERIC_LITERAL, me->source, SPAN_ShiftByChars(startingPosition, i), "More decimal places than allowed for data type currency")
                        return NUMBER_LITERAL_ERROR | NUMBER_VALUE_NONE;
                    }
                    // any other type is allowed as many places as needed
                    continue;
                }

                // allow decimal points if we didnt already come across one
                if (buffer[i] == '.') {
                    if (!foundDecimalPoint) {
                        foundDecimalPoint = true;
                        continue;
                    }

                    ERROR_AT(SUB_LEXER, ERR_LX_UNEXPECTED_CHAR_IN_NUMERIC_LITERAL, me->source, SPAN_ShiftByChars(startingPosition, i), "Multiple decimal points in decimal literal")
                    return NUMBER_LITERAL_ERROR | NUMBER_VALUE_NONE;
                }

                // for everything else: fail
                ERROR_AT(SUB_LEXER, ERR_LX_UNEXPECTED_CHAR_IN_NUMERIC_LITERAL, me->source, SPAN_ShiftByChars(startingPosition, i), "Unexpected character in decimal literal")
                return NUMBER_LITERAL_ERROR | NUMBER_VALUE_NONE;
            break;

            // ---------------------------------------------------------------------------------------------------------
            // scientific notation
            case NUMBER_LITERAL_SCIENTIFIC:
                // allow digits 0...9
                if (isdigit(buffer[i])) continue;

                // allow at max one decimal point before the exponential marker
                if (buffer[i] == '.') {
                    if (!foundDecimalPoint && !foundE) {
                        foundDecimalPoint = true;
                        continue;
                    }

                    // looks like we already came across a decimal point
                    if (!foundE) {
                        ERROR_AT(SUB_LEXER, ERR_LX_UNEXPECTED_CHAR_IN_NUMERIC_LITERAL, me->source, SPAN_ShiftByChars(startingPosition, i), "Multiple decimal points in scientific literal")
                        return NUMBER_LITERAL_ERROR | NUMBER_VALUE_NONE;
                    }

                    // looks like we found a decimal point in the whole number part of the literal
                    ERROR_AT(SUB_LEXER, ERR_LX_UNEXPECTED_CHAR_IN_NUMERIC_LITERAL, me->source, SPAN_ShiftByChars(startingPosition, i), "Unexpected decimal point in whole number exponent")
                    return NUMBER_LITERAL_ERROR | NUMBER_VALUE_NONE;
                }

                // allow at max one exponent marker
                if (buffer[i] == 'E' || buffer[i] == 'e') {
                    if (!foundE) {
                        foundE = true;
                        continue;
                    }

                    ERROR_AT(SUB_LEXER, ERR_LX_UNEXPECTED_CHAR_IN_NUMERIC_LITERAL, me->source, SPAN_ShiftByChars(startingPosition, i), "Multiple exponent markers in scientific literal")
                    return NUMBER_LITERAL_ERROR | NUMBER_VALUE_NONE;
                }

                // allow + and - right after an exponent marker
                if (buffer[i] == '+' || buffer[i] == '-') {
                    if (i > 0 && (buffer[i - 1] == 'E' || buffer[i - 1] == 'e')) {
                        continue;
                    }

                    ERROR_AT(SUB_LEXER, ERR_LX_UNEXPECTED_CHAR_IN_NUMERIC_LITERAL, me->source, SPAN_ShiftByChars(startingPosition, i), "Unexpected sign operator in scientific literal")
                    return NUMBER_LITERAL_ERROR | NUMBER_VALUE_NONE;
                }

                // for everything else: fail
                ERROR_AT(SUB_LEXER, ERR_LX_UNEXPECTED_CHAR_IN_NUMERIC_LITERAL, me->source, SPAN_ShiftByChars(startingPosition, i), "Unexpected character in scientific literal")
                return NUMBER_LITERAL_ERROR | NUMBER_VALUE_NONE;
            break;

            // ---------------------------------------------------------------------------------------------------------
            // hexadecimal literals
            case NUMBER_LITERAL_HEX:
                // allow &H at the very beginning of the literal
                if (i == 0 && buffer[i] == '&') continue;
                if (i == 1 && (buffer[i] == 'H' || buffer[i] == 'h')) continue;

                // allow 0...9 and a...f A...F
                if (isdigit(buffer[i])) continue;
                if (buffer[i] == 'A' || buffer[i] == 'a') continue;
                if (buffer[i] == 'B' || buffer[i] == 'b') continue;
                if (buffer[i] == 'C' || buffer[i] == 'c') continue;
                if (buffer[i] == 'D' || buffer[i] == 'd') continue;
                if (buffer[i] == 'E' || buffer[i] == 'e') continue;
                if (buffer[i] == 'F' || buffer[i] == 'f') continue;

                // for everything else: fail
                ERROR_AT(SUB_LEXER, ERR_LX_UNEXPECTED_CHAR_IN_NUMERIC_LITERAL, me->source, SPAN_ShiftByChars(startingPosition, i), "Unexpected character in hexadecimal literal")
                return NUMBER_LITERAL_ERROR | NUMBER_VALUE_NONE;
            break;

            // ---------------------------------------------------------------------------------------------------------
            // octal literals
            case NUMBER_LITERAL_OCT:
                // allow &O at the very beginning of the literal
                if (i == 0 && buffer[i] == '&') continue;
                if (i == 1 && (buffer[i] == 'O' || buffer[i] == 'o')) continue;

                // allow 0...7
                if (buffer[i] >= '0' && buffer[i] <= '7') continue;

                // for everything else: fail
                ERROR_AT(SUB_LEXER, ERR_LX_UNEXPECTED_CHAR_IN_NUMERIC_LITERAL, me->source, SPAN_ShiftByChars(startingPosition, i), "Unexpected character in octal literal")
                return NUMBER_LITERAL_ERROR | NUMBER_VALUE_NONE;
            break;

            // ---------------------------------------------------------------------------------------------------------
            // binary literals
            case NUMBER_LITERAL_BIN:
                // allow &B at the very beginning of the literal
                if (i == 0 && buffer[i] == '&') continue;
                if (i == 1 && (buffer[i] == 'B' || buffer[i] == 'b')) continue;

                // allow 0/1
                if (buffer[i] == '0' || buffer[i] == '1') continue;

                // for everything else: fail
                ERROR_AT(SUB_LEXER, ERR_LX_UNEXPECTED_CHAR_IN_NUMERIC_LITERAL, me->source, SPAN_ShiftByChars(startingPosition, i), "Unexpected character in binary literal")
                return NUMBER_LITERAL_ERROR | NUMBER_VALUE_NONE;
            break;

            default: ;
        }
    }

    return lexingType | valueType | (hasTypeSpecifier ? NUMBER_LITERAL_HAS_EXPLICIT_TYPE : 0);
}

bool LEXER_convertIntoNumber(lexer_t *me, token_t *token, number_literal_type_t literalType, const char *buffer, uint32_t length, span_t errorSpan) {
    char workingBuffer[MAX_TOKEN_LENGTH + 5];
    uint32_t workingPosition = 0;

    // filter out any underscores
    for (int i = 0; i < length; ++i) {
        if (buffer[i] == '_') continue;
        workingBuffer[workingPosition++] = buffer[i];
    }
    workingBuffer[workingPosition] = 0;

    // normal integers, hex, oct and bin
    if (literalType & NUMBER_LITERAL_INT ||
        literalType & NUMBER_LITERAL_HEX || literalType & NUMBER_LITERAL_OCT || literalType & NUMBER_LITERAL_BIN) {

        uint64_t value = 0;
        if (literalType & NUMBER_LITERAL_INT) value = strtoull(workingBuffer, NULL, 10);
        if (literalType & NUMBER_LITERAL_HEX) value = strtoull(workingBuffer+2, NULL, 16);
        if (literalType & NUMBER_LITERAL_OCT) value = strtoull(workingBuffer+2, NULL, 8);
        if (literalType & NUMBER_LITERAL_BIN) value = strtoull(workingBuffer+2, NULL, 2);

        // if the type of this literal has been set explicitly, eg. if its not allowed to grow if necessary
        // -> make sure it fits
        if (literalType & NUMBER_LITERAL_HAS_EXPLICIT_TYPE) {

            // if this literal is a decimal number -> check against signed sizes
            if (literalType & NUMBER_LITERAL_INT) {
                CHECK_DATA_TYPE_COMPATABILITY(NUMBER_VALUE_BYTE     , MAX_VALUE_BYTE            , "Byte")
                CHECK_DATA_TYPE_COMPATABILITY(NUMBER_VALUE_INT      , MAX_VALUE_SIGNED_INT      , "Integer")
                CHECK_DATA_TYPE_COMPATABILITY(NUMBER_VALUE_LONG     , MAX_VALUE_SIGNED_LONG     , "Long")
                CHECK_DATA_TYPE_COMPATABILITY(NUMBER_VALUE_LONG_LONG, MAX_VALUE_SIGNED_LONG_LONG, "LongLong")
                CHECK_DATA_TYPE_COMPATABILITY(NUMBER_VALUE_SINGLE   , MAX_VALUE_SINGLE          , "Single")
                //_CHECK_DATA_TYPE_COMPATABILITY(NUMBER_VALUE_DOUBLE   , MAX_VALUE_DOUBLE          , "Double")
                CHECK_DATA_TYPE_COMPATABILITY(NUMBER_VALUE_CURRENCY , MAX_VALUE_CURRENCY_WHOLE  , "Currency")
            }

            // for hex, oct and bin check against unsigned sizes
            else {
                CHECK_DATA_TYPE_COMPATABILITY(NUMBER_VALUE_BYTE     , MAX_VALUE_BYTE              , "Byte")
                CHECK_DATA_TYPE_COMPATABILITY(NUMBER_VALUE_INT      , MAX_VALUE_UNSIGNED_INT      , "Integer")
                CHECK_DATA_TYPE_COMPATABILITY(NUMBER_VALUE_LONG     , MAX_VALUE_UNSIGNED_LONG     , "Long")
                CHECK_DATA_TYPE_COMPATABILITY(NUMBER_VALUE_LONG_LONG, MAX_VALUE_UNSIGNED_LONG_LONG, "LongLong")
            }
        }
        else {
            // if this literal is a decimal number -> check against signed sizes
            if (literalType & NUMBER_LITERAL_INT) {

                // otherwise, grow the type if needed
                GROW_DATA_TYPE_IF_NEEDED(NUMBER_VALUE_BYTE, MAX_VALUE_BYTE       , NUMBER_VALUE_INT);
                GROW_DATA_TYPE_IF_NEEDED(NUMBER_VALUE_INT , MAX_VALUE_SIGNED_INT , NUMBER_VALUE_LONG);
                GROW_DATA_TYPE_IF_NEEDED(NUMBER_VALUE_LONG, MAX_VALUE_SIGNED_LONG, NUMBER_VALUE_LONG_LONG);

                // make sure this actually fits in a long long
                CHECK_DATA_TYPE_COMPATABILITY(NUMBER_VALUE_LONG_LONG, MAX_VALUE_SIGNED_LONG_LONG, "LongLong")
            }

            // for hex, oct and bin check against unsigned sizes
            else {

                // otherwise, grow the type if needed
                GROW_DATA_TYPE_IF_NEEDED(NUMBER_VALUE_BYTE, MAX_VALUE_BYTE         , NUMBER_VALUE_INT);
                GROW_DATA_TYPE_IF_NEEDED(NUMBER_VALUE_INT , MAX_VALUE_UNSIGNED_INT , NUMBER_VALUE_LONG);
                GROW_DATA_TYPE_IF_NEEDED(NUMBER_VALUE_LONG, MAX_VALUE_UNSIGNED_LONG, NUMBER_VALUE_LONG_LONG);

                // make sure this actually fits in a long long
                CHECK_DATA_TYPE_COMPATABILITY(NUMBER_VALUE_LONG_LONG, MAX_VALUE_UNSIGNED_LONG_LONG, "LongLong")
            }
        }

        // otherwise, allocate the buffer for this value and store it

        // use signed types for integer literals
        if (literalType & NUMBER_LITERAL_INT) {
            switch (literalType & 0xFF00) {
                ALLOCATE_AND_CONVERT(NUMBER_VALUE_BYTE, uint8_t)
                ALLOCATE_AND_CONVERT(NUMBER_VALUE_INT, int16_t)
                ALLOCATE_AND_CONVERT(NUMBER_VALUE_LONG, int32_t)
                ALLOCATE_AND_CONVERT(NUMBER_VALUE_LONG_LONG, int64_t)

                ALLOCATE_AND_CONVERT(NUMBER_VALUE_SINGLE, float)
                ALLOCATE_AND_CONVERT(NUMBER_VALUE_DOUBLE, double)

                case NUMBER_VALUE_CURRENCY:
                    token->value = malloc(sizeof(int64_t));
                    *(int64_t*)token->value = value * 10'000;
                break;

                default: ;
            }
        }

        // use unsigned types for hex, oct and bin literals
        else {
            switch (literalType & 0xFF00) {
                ALLOCATE_AND_CONVERT(NUMBER_VALUE_BYTE, uint8_t)
                ALLOCATE_AND_CONVERT(NUMBER_VALUE_INT, uint16_t)
                ALLOCATE_AND_CONVERT(NUMBER_VALUE_LONG, uint32_t)
                ALLOCATE_AND_CONVERT(NUMBER_VALUE_LONG_LONG, uint64_t)
                default: ;
            }
        }

        // keep track of the current number type in case any conversions took place
        token->numberValueType = literalType & 0xFF00;

        // coolio :)
        return true;
    }

    // normal decimals and scientific notation
    if (literalType & NUMBER_LITERAL_DECIMAL || literalType & NUMBER_LITERAL_SCIENTIFIC) {

        // singles and doubles are handled differently to currency values
        if (literalType & NUMBER_VALUE_SINGLE || literalType & NUMBER_VALUE_DOUBLE) {
            double value = strtod(workingBuffer, NULL);

            // if the type of this literal has been set explicitly, eg. if its not allowed to grow if necessary
            // -> make sure it fits
            if (literalType & NUMBER_LITERAL_HAS_EXPLICIT_TYPE) {
                CHECK_DATA_TYPE_COMPATABILITY(NUMBER_VALUE_SINGLE, MAX_VALUE_SINGLE, "Single")
            }

            // otherwise, grow the type if needed
            else {
                GROW_DATA_TYPE_IF_NEEDED(NUMBER_VALUE_SINGLE, MAX_VALUE_SINGLE, NUMBER_VALUE_DOUBLE)
            }

            // otherwise, allocate the buffer for this value and store it
            switch (literalType & 0xFF00) {
                ALLOCATE_AND_CONVERT(NUMBER_VALUE_SINGLE, float)
                ALLOCATE_AND_CONVERT(NUMBER_VALUE_DOUBLE, double)
                default:;
            }

            // keep track of the current number type in case any conversions took place
            token->numberValueType = literalType & 0xFF00;

            // done
            return true;
        }

        // currency values
        if (literalType & NUMBER_VALUE_CURRENCY) {
            int numDecimalPlaces = 0;
            bool pastDecimalPoint = false;

            // filter out any underscores and decimal points
            workingPosition = 0;
            for (int i = 0; i < length; ++i) {
                if (buffer[i] == '@') continue;
                if (buffer[i] == '_') continue;
                if (buffer[i] == '.') {
                    pastDecimalPoint = true;
                    continue;
                }

                workingBuffer[workingPosition++] = buffer[i];
                if (pastDecimalPoint) numDecimalPlaces++;
            }

            // make sure we always have 4 decimal places
            while (numDecimalPlaces < 4) {
                workingBuffer[workingPosition++] = '0';
                numDecimalPlaces++;
            }

            // terminate the buffer
            workingBuffer[workingPosition] = 0;

            // convert the buffer into a number
            uint64_t value = strtoull(workingBuffer, NULL, 10);

            // make sure this fits
            CHECK_DATA_TYPE_COMPATABILITY(NUMBER_VALUE_CURRENCY, MAX_VALUE_CURRENCY_FRACTIONAL, "Currency")

            // if it does, allocate the buffer and store the value
            switch (literalType & 0xFF00) {
                ALLOCATE_AND_CONVERT(NUMBER_VALUE_CURRENCY, int64_t)
                default:;
            }

            // keep track of the current number type in case any conversions took place
            token->numberValueType = literalType & 0xFF00;

            // worked
            return true;
        }
    }

    ERROR_AT(SUB_LEXER, ERR_INTERNAL, me->source, errorSpan, "LiteralType/ValueType combination not accounted for!")
    return false;
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
    token.numberValueType = NUMBER_VALUE_NONE;

    return token;
}

// ---------------------------------------------------------------------------------------------------------------------
// LEXER_copyTokenValue:
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