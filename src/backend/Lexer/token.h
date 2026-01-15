//
// Created by ds on 12/21/25.
//

#ifndef TOKEN_H
#define TOKEN_H
#include <stdint.h>
#include <stdbool.h>

#define MAX_TOKEN_LENGTH 2000
#define MAX_IDENTIFIER_LENGTH 255

#define MAX_VALUE_BYTE 255
#define MAX_VALUE_SIGNED_INT 32'767
#define MAX_VALUE_UNSIGNED_INT 65'535
#define MAX_VALUE_SIGNED_LONG 2'147'483'647
#define MAX_VALUE_UNSIGNED_LONG 4'294'967'295
#define MAX_VALUE_SIGNED_LONG_LONG 9'223'372'036'854'775'807
#define MAX_VALUE_UNSIGNED_LONG_LONG 18'446'744'073'709'551'615UL

#define MAX_VALUE_SINGLE 3.402823E38
#define MAX_VALUE_DOUBLE 1.79769313486232E308
#define MAX_VALUE_CURRENCY_WHOLE 922'337'203'685'477
#define MAX_VALUE_CURRENCY_FRACTIONAL 922'337'203'685'477'5807

// ---------------------------------------------------------------------------------------------------------------------
// All our different types of tokens!

#define FOREACH_TOKEN_TYPE(GEN)   \
    GEN(TK_ERROR)                 \
                                  \
    GEN(TK_EOF)                   \
    GEN(TK_EOS)                   \
                                  \
    GEN(TK_KW_BEGIN)              \
    GEN(TK_KW_END)                \
    GEN(TK_KW_SUB)                \
    GEN(TK_KW_FUNCTION)           \
    GEN(TK_KW_TYPE)               \
    GEN(TK_KW_MODULE)             \
                                  \
    GEN(TK_KW_PUBLIC)             \
    GEN(TK_KW_PRIVATE)            \
    GEN(TK_KW_FRIEND)             \
    GEN(TK_KW_STATIC)             \
                                  \
    GEN(TK_KW_OPTIONAL)           \
    GEN(TK_KW_BYVAL)              \
    GEN(TK_KW_BYREF)              \
    GEN(TK_KW_PARAMARRAY)         \
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
    GEN(TK_KW_LET)                \
    GEN(TK_KW_SET)                \
                                  \
    GEN(TK_KW_FOR)                \
    GEN(TK_KW_STEP)               \
    GEN(TK_KW_NEXT)               \
                                  \
    GEN(TK_KW_WHILE)              \
    GEN(TK_KW_WEND)               \
                                  \
    GEN(TK_KW_IF)                 \
    GEN(TK_KW_ELSEIF)             \
    GEN(TK_KW_THEN)               \
    GEN(TK_KW_ELSE)               \
                                  \
    GEN(TK_KW_SELECT)             \
    GEN(TK_KW_CASE)               \
                                  \
    GEN(TK_KW_GOTO)               \
                                  \
    GEN(TK_PC_OPEN_PARENTHESIS)   \
    GEN(TK_PC_CLOSED_PARENTHESIS) \
    GEN(TK_PC_COMMA)              \
    GEN(TK_PC_PERIOD)             \
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
    GEN(TK_LT_BOOLEAN)            \
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
// All supported types of number literals

// Lexing types, all the different ways number literals can be lexed
typedef enum NUMBER_LITERAL_LEXING_TYPE {

    // invalid number literal
    NUMBER_LITERAL_ERROR      = 0b00000001,

    // the basics
    NUMBER_LITERAL_INT        = 0b00000010,
    NUMBER_LITERAL_DECIMAL    = 0b00000100,

    // scientific notation
    NUMBER_LITERAL_SCIENTIFIC = 0b00001000,

    // hex, oct and binary literals
    NUMBER_LITERAL_HEX        = 0b00010000,
    NUMBER_LITERAL_OCT        = 0b00100000,
    NUMBER_LITERAL_BIN        = 0b01000000,

} number_literal_lexing_type_t;

// Value types, all the different data types that can be represented by a numeric literal
typedef enum NUMBER_LITERAL_VALUE_TYPE {

    // invalid number literal
    NUMBER_VALUE_NONE      = 0b00000001'00000000,

    // integers
    NUMBER_VALUE_BYTE      = 0b00000010'00000000,
    NUMBER_VALUE_INT       = 0b00000100'00000000,
    NUMBER_VALUE_LONG      = 0b00001000'00000000,
    NUMBER_VALUE_LONG_LONG = 0b00010000'00000000,

    // decimals
    NUMBER_VALUE_SINGLE    = 0b00100000'00000000,
    NUMBER_VALUE_DOUBLE    = 0b01000000'00000000,
    NUMBER_VALUE_CURRENCY  = 0b10000000'00000000,

} number_literal_value_type_t;

// marker for when the type of a literal has been set explicitly
#define NUMBER_LITERAL_HAS_EXPLICIT_TYPE 0b10000000

typedef uint16_t number_literal_type_t;

// ---------------------------------------------------------------------------------------------------------------------
// List of all keywords in the order they are listed in above
static const char *KEYWORDS[] = {
    "begin",
    "end",
    "sub",
    "function",
    "type",
    "module",
    "public",
    "private",
    "friend",
    "static",
    "optional",
    "byval",
    "byref",
    "paramarray",
    "call",
    "exit",
    "dim",
    "redim",
    "preserve",
    "to",
    "as",
    "let",
    "set",
    "for",
    "step",
    "next",
    "while",
    "wend",
    "if",
    "elseif",
    "then",
    "else",
    "select",
    "case",
    "goto",
};
#define NUM_KEYWORDS (sizeof(KEYWORDS) / sizeof(KEYWORDS[0]))

// ---------------------------------------------------------------------------------------------------------------------
// List of all punctuation marks and operators in the order they are listed in above
static const char PUNCTUATION_AND_OPERATORS[] = {
    '(', ')', ',', '.',
    '+', '-', '*', '/', '=', '&'
};
#define NUM_PUNC_AND_OPS (sizeof(PUNCTUATION_AND_OPERATORS) / sizeof(PUNCTUATION_AND_OPERATORS[0]))

// ---------------------------------------------------------------------------------------------------------------------
// Struct for our lexed tokens
typedef struct TOKEN {
    token_type_t type;
    char *strValue; // the actual text of this token taken from the source, this is mostly just important for identifiers

    void *value;
    number_literal_value_type_t numberValueType;

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
