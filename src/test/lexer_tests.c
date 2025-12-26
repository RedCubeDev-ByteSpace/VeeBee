//
// Created by ds on 12/23/25.
//
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"
#include "Lexer/lexer.h"
#include "Lexer/source.h"
#include "Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------

DEFINE_TEST(TEST_LX_LEX_all_keywords)
DEFINE_TEST(TEST_LX_LEX_all_keywords_upper)
DEFINE_TEST(TEST_LX_LEX_all_punctuation)
DEFINE_TEST(TEST_LX_LEX_identifier)
DEFINE_TEST(TEST_LX_LEX_identifier_toolong)
DEFINE_TEST(TEST_LX_LEX_string)
DEFINE_TEST(TEST_LX_LEX_string_toolong)
DEFINE_TEST(TEST_LX_LEX_string_unterminated)
DEFINE_TEST(TEST_LX_LEX_number)
DEFINE_TEST(TEST_LX_LEX_number_toolong)
DEFINE_TEST(TEST_LX_LEX_comment)
DEFINE_TEST(TEST_LX_LEX_newline)

// ---------------------------------------------------------------------------------------------------------------------
BEGIN_TEST_SET(LEXER)
    RUN_TEST(TEST_LX_LEX_all_keywords)
    RUN_TEST(TEST_LX_LEX_all_keywords_upper)
    RUN_TEST(TEST_LX_LEX_all_punctuation)
    RUN_TEST(TEST_LX_LEX_identifier)
    RUN_TEST(TEST_LX_LEX_identifier_toolong)
    RUN_TEST(TEST_LX_LEX_string)
    RUN_TEST(TEST_LX_LEX_string_toolong)
    RUN_TEST(TEST_LX_LEX_string_unterminated)
    RUN_TEST(TEST_LX_LEX_number)
    RUN_TEST(TEST_LX_LEX_number_toolong)
    RUN_TEST(TEST_LX_LEX_comment)
    RUN_TEST(TEST_LX_LEX_newline)
END_TEST_SET
// ---------------------------------------------------------------------------------------------------------------------

BEGIN_TEST(TEST_LX_LEX_all_keywords)

    // create a buffer to store all keywords with whitespace inbetween
    uint32_t bufferSize = 0;
    for (int i = 0; i < NUM_KEYWORDS; ++i)
        bufferSize += strlen(KEYWORDS[i]) + 1;

    // allocate the buffer
    char *buffer = (char *)malloc(bufferSize + 1);
    uint32_t idx = 0;

    // copy over each keyword
    for (int i = 0; i < NUM_KEYWORDS; ++i) {
        memcpy(buffer + idx, KEYWORDS[i], strlen(KEYWORDS[i]));
        idx += strlen(KEYWORDS[i]);

        // add spaces in between
        buffer[idx] = ' ';
        idx++;
    }

    // cap the buffer off with a null terminator
    buffer[idx] = '\0';

    // create a new source from this buffer
    source_t source = SOURCE_Init_FromText(buffer);
    lexer_t *lexer = LEXER_Init(source);

    // run through the lexing
    LEXER_Lex(lexer);

    TEST_ASSERT(lexer->tokens.length == NUM_KEYWORDS + 1);

    for (int i = 0; i < lexer->tokens.length-1; ++i) {
        TEST_ASSERT(lexer->tokens.tokens[i].type == TK_KW_BEGIN + i);
    }

    LX_TOKEN_LIST_Unload(lexer->tokens);
    LEXER_Unload(lexer);
    SOURCE_Unload(source);
    free(buffer);
END_TEST

BEGIN_TEST(TEST_LX_LEX_all_keywords_upper)

    // create a buffer to store all keywords with whitespace inbetween
    uint32_t bufferSize = 0;
    for (int i = 0; i < NUM_KEYWORDS; ++i)
        bufferSize += strlen(KEYWORDS[i]) + 1;

    // allocate the buffer
    char *buffer = (char *)malloc(bufferSize + 1);
    uint32_t idx = 0;

    // copy over each keyword
    for (int i = 0; i < NUM_KEYWORDS; ++i) {
        memcpy(buffer + idx, KEYWORDS[i], strlen(KEYWORDS[i]));
        idx += strlen(KEYWORDS[i]);

        // add spaces in between
        buffer[idx] = ' ';
        idx++;
    }

    // cap the buffer off with a null terminator
    buffer[idx] = '\0';

    // capitalize the buffer
    for (int i = 0; i < bufferSize; ++i) {
        buffer[i] = toupper(buffer[i]);
    }

    // create a new source from this buffer
    source_t source = SOURCE_Init_FromText(buffer);
    lexer_t *lexer = LEXER_Init(source);

    // run through the lexing
    LEXER_Lex(lexer);

    TEST_ASSERT(lexer->tokens.length == NUM_KEYWORDS + 1);

    for (int i = 0; i < lexer->tokens.length-1; ++i) {
        TEST_ASSERT(lexer->tokens.tokens[i].type == TK_KW_BEGIN + i);
    }

    LX_TOKEN_LIST_Unload(lexer->tokens);
    LEXER_Unload(lexer);
    SOURCE_Unload(source);
    free(buffer);
END_TEST

BEGIN_TEST(TEST_LX_LEX_all_punctuation)
    // create a buffer to store all punctuation marks with whitespace inbetween
    uint32_t bufferSize = NUM_PUNC_AND_OPS * 2;

    // allocate the buffer
    char *buffer = (char *)malloc(bufferSize + 1);
    uint32_t idx = 0;

    // copy over each keyword
    for (int i = 0; i < NUM_PUNC_AND_OPS; ++i) {
        buffer[idx] = PUNCTUATION_AND_OPERATORS[i];
        idx++;

        // add spaces in between
        buffer[idx] = ' ';
        idx++;
    }

    // cap the buffer off with a null terminator
    buffer[idx] = '\0';

    // create a new source from this buffer
    source_t source = SOURCE_Init_FromText(buffer);
    lexer_t *lexer = LEXER_Init(source);

    // run through the lexing
    LEXER_Lex(lexer);

    TEST_ASSERT(lexer->tokens.length == NUM_PUNC_AND_OPS + 1);

    for (int i = 0; i < lexer->tokens.length-1; ++i) {
        TEST_ASSERT(lexer->tokens.tokens[i].type == TK_PC_OPEN_PARENTHESIS + i);
    }

    LX_TOKEN_LIST_Unload(lexer->tokens);
    LEXER_Unload(lexer);
    SOURCE_Unload(source);
    free(buffer);
END_TEST

BEGIN_TEST(TEST_LX_LEX_identifier)
    // Test buffer of some identifiers with mixed capitalisation
    source_t source = SOURCE_Init_FromText("very NICE i_DENtifieRs");
    lexer_t *lexer = LEXER_Init(source);

    LEXER_Lex(lexer);

    // we expect three tokens as well as an end-of-statement
    TEST_ASSERT(lexer->tokens.length == 4)

    TEST_ASSERT(lexer->tokens.tokens[0].type == TK_IDENTIFIER);
    TEST_ASSERT(strcmp(lexer->tokens.tokens[0].strValue, "very") == 0);

    TEST_ASSERT(lexer->tokens.tokens[1].type == TK_IDENTIFIER);
    TEST_ASSERT(strcmp(lexer->tokens.tokens[1].strValue, "nice") == 0);

    TEST_ASSERT(lexer->tokens.tokens[2].type == TK_IDENTIFIER);
    TEST_ASSERT(strcmp(lexer->tokens.tokens[2].strValue, "i_dentifiers") == 0);

    TEST_ASSERT(lexer->tokens.tokens[3].type == TK_EOS);

    LX_TOKEN_LIST_Unload(lexer->tokens);
    LEXER_Unload(lexer);
    SOURCE_Unload(source);
END_TEST

BEGIN_TEST(TEST_LX_LEX_identifier_toolong)

    // test the handling of too long identifiers (>255)
    source_t source = SOURCE_Init_FromText("abc this_is_a_very_long_identifier_name_that_should_be_rejected_by_the_lexer_and_should_be_truncated_to_fit_into_255_characters_because_its_against_the_laguage_spec_but_like_seriously_why_would_an_identifier_ever_be_this_long_like_what_the_fuck_but_like_ok_man_you_do_you_honestly_i_dont_care_maybe_this_is_actually_sick_af_to_use");
    lexer_t *lexer = LEXER_Init(source);

    LEXER_Lex(lexer);

    // we expect two tokens as well as an end-of-statement
    TEST_ASSERT(lexer->tokens.length == 3)

    TEST_ASSERT(lexer->tokens.tokens[0].type == TK_IDENTIFIER);
    TEST_ASSERT(strcmp(lexer->tokens.tokens[0].strValue, "abc") == 0);

    // make sure the truncation was done correctly
    TEST_ASSERT(lexer->tokens.tokens[1].type == TK_IDENTIFIER);
    TEST_ASSERT(strlen(lexer->tokens.tokens[1].strValue) == 255);
    TEST_ASSERT(strcmp(lexer->tokens.tokens[1].strValue, "this_is_a_very_long_identifier_name_that_should_be_rejected_by_the_lexer_and_should_be_truncated_to_fit_into_255_characters_because_its_against_the_laguage_spec_but_like_seriously_why_would_an_identifier_ever_be_this_long_like_what_the_fuck_but_like_ok_ma") == 0);

    TEST_ASSERT(lexer->tokens.tokens[2].type == TK_EOS);

    LX_TOKEN_LIST_Unload(lexer->tokens);
    LEXER_Unload(lexer);
    SOURCE_Unload(source);
END_TEST

BEGIN_TEST(TEST_LX_LEX_string)

    // Test buffer of some identifiers with mixed capitalisation
    source_t source = SOURCE_Init_FromText("\"very\" \"NICE\" \"s_TRings\"");
    lexer_t *lexer = LEXER_Init(source);

    LEXER_Lex(lexer);

    // we expect three tokens as well as an end-of-statement
    TEST_ASSERT(lexer->tokens.length == 4)

    TEST_ASSERT(lexer->tokens.tokens[0].type == TK_LT_STRING);
    TEST_ASSERT(strcmp(lexer->tokens.tokens[0].strValue, "very") == 0);

    TEST_ASSERT(lexer->tokens.tokens[1].type == TK_LT_STRING);
    TEST_ASSERT(strcmp(lexer->tokens.tokens[1].strValue, "NICE") == 0);

    TEST_ASSERT(lexer->tokens.tokens[2].type == TK_LT_STRING);
    TEST_ASSERT(strcmp(lexer->tokens.tokens[2].strValue, "s_TRings") == 0);

    TEST_ASSERT(lexer->tokens.tokens[3].type == TK_EOS);

    LX_TOKEN_LIST_Unload(lexer->tokens);
    LEXER_Unload(lexer);
    SOURCE_Unload(source);
END_TEST

BEGIN_TEST(TEST_LX_LEX_string_toolong)

    // create a "massive" text buffer
    const int stringLength = 2500;
    char *longStringSource = malloc(stringLength + 3);
    longStringSource[0] = '"';
    longStringSource[stringLength+1] = '"';
    longStringSource[stringLength+2] = 0;

    for (int i = 0; i < stringLength; ++i) {
        longStringSource[i + 1] = 'o';
    }

    // Test buffer of some identifiers with mixed capitalisation
    source_t source = SOURCE_Init_FromText(longStringSource);
    lexer_t *lexer = LEXER_Init(source);

    LEXER_Lex(lexer);

    // we expect one token as well as an end-of-statement
    TEST_ASSERT(lexer->tokens.length == 2)

    TEST_ASSERT(lexer->tokens.tokens[0].type == TK_LT_STRING);
    TEST_ASSERT(strlen(lexer->tokens.tokens[0].strValue) == 2000);

    TEST_ASSERT(lexer->tokens.tokens[1].type == TK_EOS);

    LX_TOKEN_LIST_Unload(lexer->tokens);
    LEXER_Unload(lexer);
    SOURCE_Unload(source);
    free(longStringSource);
END_TEST

BEGIN_TEST(TEST_LX_LEX_string_unterminated)

    // Test buffer of some identifiers with mixed capitalisation
    source_t source = SOURCE_Init_FromText("Begin Sub \"not exactly closed string End Sub");
    lexer_t *lexer = LEXER_Init(source);

    LEXER_Lex(lexer);

    // we expect three tokens as well as an end-of-statement
    TEST_ASSERT(lexer->tokens.length == 4)

    TEST_ASSERT(lexer->tokens.tokens[0].type == TK_KW_BEGIN);
    TEST_ASSERT(lexer->tokens.tokens[1].type == TK_KW_SUB);

    TEST_ASSERT(lexer->tokens.tokens[2].type == TK_LT_STRING);
    TEST_ASSERT(strcmp(lexer->tokens.tokens[2].strValue, "not exactly closed string End Sub") == 0);

    TEST_ASSERT(lexer->tokens.tokens[3].type == TK_EOS);

    LX_TOKEN_LIST_Unload(lexer->tokens);
    LEXER_Unload(lexer);
    SOURCE_Unload(source);
END_TEST

BEGIN_TEST(TEST_LX_LEX_number)

// create a source with a few different numbers
source_t source = SOURCE_Init_FromText("");

END_TEST

BEGIN_TEST(TEST_LX_LEX_number_toolong)
TEST_IGNORE()
END_TEST

BEGIN_TEST(TEST_LX_LEX_comment)
TEST_IGNORE()
END_TEST

BEGIN_TEST(TEST_LX_LEX_newline)
TEST_IGNORE()
END_TEST
