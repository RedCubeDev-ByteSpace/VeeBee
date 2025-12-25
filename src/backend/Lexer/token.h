//
// Created by ds on 12/21/25.
//

#ifndef TOKEN_H
#define TOKEN_H
#include <stdint.h>
#include <stdbool.h>

#define MAX_TOKEN_LENGTH 2000
#define MAX_IDENTIFIER_LENGTH 255

// ---------------------------------------------------------------------------------------------------------------------
// All our different types of tokens!

#define FOREACH_TOKEN_TYPE(GEN)   \
    GEN(TK_EOF)                   \
    GEN(TK_EOS)                   \
                                  \
    GEN(TK_KW_BEGIN)              \
    GEN(TK_KW_END)                \
    GEN(TK_KW_SUB)                \
    GEN(TK_KW_FUNCTION)           \
    GEN(TK_KW_TYPE)               \
                                  \
    GEN(TK_KW_CALL)               \
    GEN(TK_KW_EXIT)               \
                                  \
    GEN(TK_KW_DIM)                \
    GEN(TK_KW_REDIM)              \
    GEN(TK_KW_PRESERVE)           \
    GEN(TK_KW_TO)                 \
    GEN(TK_KW_AS)                 \
                                  \
    GEN(TK_PC_OPEN_PARENTHESIS)   \
    GEN(TK_PC_CLOSED_PARENTHESIS) \
    GEN(TK_PC_COMMA)              \
                                  \
    GEN(TK_OP_PLUS)               \
    GEN(TK_OP_MINUS)              \
    GEN(TK_OP_STAR)               \
    GEN(TK_OP_SLASH)              \
    GEN(TK_OP_EQUALS)             \
    GEN(TK_OP_AND)                \
                                  \
    GEN(TK_LT_STRING)             \
    GEN(TK_LT_NUMBER)             \
                                  \
    GEN(TK_IDENTIFIER)            \

#define GEN_ENUM(ENUM) ENUM,
#define GEN_STRING(STRING) #STRING,

// the actual enum for all our types
typedef enum TOKEN_TYPE {
    FOREACH_TOKEN_TYPE(GEN_ENUM)
} token_type_t;

// names for all these types for debug output
static const char *TOKEN_TYPE_STRING[] = {
    FOREACH_TOKEN_TYPE(GEN_STRING)
};

// ---------------------------------------------------------------------------------------------------------------------
// List of all keywords in the order they are listed in above
static const char *KEYWORDS[] = {
    "begin",
    "end",
    "sub",
    "function",
    "type",
    "call",
    "exit",
    "dim",
    "redim",
    "preserve",
    "to",
    "as"
};
#define NUM_KEYWORDS 11

// ---------------------------------------------------------------------------------------------------------------------
// List of all punctuation marks and operators in the order they are listed in above
static const char PUNCTUATION_AND_OPERATORS[] = {
    '(', ')', ',',
    '+', '-', '*', '/', '=', '&'
};
#define NUM_PUNC_AND_OPS 9

// ---------------------------------------------------------------------------------------------------------------------
// Struct for our lexed tokens
typedef struct TOKEN {
    token_type_t type;
    char *strValue; // the actual text of this token taken from the source, this is mostly just important for identifiers
    void *value;

    uint64_t srcPos;
    uint32_t length;
    uint32_t line;
    uint32_t column;
} token_t;

// ---------------------------------------------------------------------------------------------------------------------
// Dynamic size list for our tokens
#define LX_TOKEN_LIST_GROWTH 8
typedef struct TOKEN_LIST {
    token_t *tokens;
    uint32_t length;
    uint32_t capacity;
} token_list_t;

token_list_t LX_TOKEN_LIST_Init();
void LX_TOKEN_LIST_Unload(token_list_t me);
void LX_TOKEN_LIST_Add(token_list_t *me, token_t newToken);
bool LX_TOKEN_LIST_grow(token_list_t *me);

#endif //TOKEN_H
