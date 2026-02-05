//
// Created by ds on 12/21/25.
//

#ifndef ERROR_H
#define ERROR_H

#include<stdio.h>

// ---------------------------------------------------------------------------------------------------------------------
// Define all VeeBee subsystems
# define FOREACH_SUBSYSTEM(GEN) \
    GEN(SUB_CLI)                \
    GEN(SUB_LEXER)              \
    GEN(SUB_PARSER)             \
    GEN(SUB_BINDER)             \

#define GEN_ENUM(ENUM) ENUM,
#define GEN_STRING(STRING) #STRING,
#include "Lexer/source.h"

// Subsystem enum
typedef enum ERR_SUBSYSTEM {
    FOREACH_SUBSYSTEM(GEN_ENUM)
} err_subsystem_t;

// Subsystem names
static const char *SUBSYSTEM_NAMES[] = {
    FOREACH_SUBSYSTEM(GEN_STRING)
};

// ---------------------------------------------------------------------------------------------------------------------
// Define all VeeBee error codes
# define FOREACH_ERROR_TYPE(GEN)                       \
    GEN(ERR_INTERNAL)                                  \
    GEN(ERR_NOT_IMPLEMENTED)                           \
                                                       \
    GEN(ERR_CL_NO_SOURCES_GIVEN)                       \
    GEN(ERR_CL_TOO_MANY_SOURCES)                       \
    GEN(ERR_CL_SOURCE_DOESNT_EXIST)                    \
                                                       \
    GEN(ERR_LX_SOURCE_TEXT_NULL)                       \
    GEN(ERR_LX_PATH_NULL)                              \
    GEN(ERR_LX_PATH_TOO_LONG)                          \
    GEN(ERR_LX_FILE_NOT_FOUND)                         \
    GEN(ERR_LX_TOKEN_VALUE_TOO_LONG)                   \
    GEN(ERR_LX_IDENTIFIER_TOO_LONG)                    \
    GEN(ERR_LX_UNTERMINATED_STRING)                    \
    GEN(ERR_LX_UNEXPECTED_CHARACTER)                   \
    GEN(ERR_LX_UNEXPECTED_LITERAL_QUALIFIER)           \
    GEN(ERR_LX_UNEXPECTED_CHAR_IN_NUMERIC_LITERAL)     \
    GEN(ERR_LX_INCOMPATIBLE_NUMERIC_TYPE_AND_LITERAL)  \
    GEN(ERR_LX_INCOMPATIBLE_VALUE_FOR_NUMERIC_TYPE)    \
                                                       \
    GEN(ERR_PS_UNEXPECTED_TOKEN)                       \
    GEN(ERR_PS_UNEXPECTED_NON_MEMBER)                  \
    GEN(ERR_PS_UNEXPECTED_NON_EXPRESSION)              \
    GEN(ERR_PS_UNEXPECTED_NON_STATEMENT)               \
    GEN(ERR_PS_UNEXPECTED_AS_CLAUSE)                   \
    GEN(ERR_PS_EXPECTED_UNARY_OPERATOR)                \
    GEN(ERR_PS_EXPECTED_BINARY_OPERATOR)               \
                                                       \
    GEN(ERR_BD_MORE_THAN_ONE_MODULE_MEMBERS)           \
    GEN(ERR_BD_MODULE_MEMBER_NOT_AT_BEGINNING_OF_FILE) \
    GEN(ERR_BD_UNRECOGNIZED_TYPE_NAME)                 \
    GEN(ERR_BD_TOO_MANY_ARRAY_DIMENSIONS)              \
    GEN(ERR_BD_NO_FIELDS_IN_TYPE)                      \
    GEN(ERR_BD_ARRAY_TYPE_MUST_BE_GENERIC)             \
    GEN(ERR_BD_UNEXPECTED_ARRAY_TYPE)                  \
    GEN(ERR_BD_NON_UNIQUE_SYMBOL)                      \
    GEN(ERR_BD_INVALID_NON_OPTIONAL_PARAMETER)         \
    GEN(ERR_BD_INVALID_PARAMARRAY_PARAMETER)           \
    GEN(ERR_BD_INVALID_NON_PARAMARRAY_PARAMETER)       \
    GEN(ERR_BD_ILLEGAL_COMBINATION_OF_MODIFIERS)       \
    GEN(ERR_BD_FRIEND_MODIFIER_NOT_ALLOWED)            \

// Error type enum
typedef enum ERR_ERROR_TYPE {
    FOREACH_ERROR_TYPE(GEN_ENUM)
} error_type_t;

// Error type names
static const char *ERROR_TYPE_NAMES[] = {
    FOREACH_ERROR_TYPE(GEN_STRING)
};

// ---------------------------------------------------------------------------------------------------------------------
// Reporting functions

#ifndef TESTING
#define ERROR(SUBSYSTEM, TYPE, MSG) error(SUBSYSTEM, TYPE, MSG);
#define ERROR_SPLICE(SUBSYSTEM, TYPE, MSG, ...) \
    MSG_SPLICE(MSG, __VA_ARGS__)                \
    error(SUBSYSTEM, TYPE, ERR_MSG_BUFFER);     \

#define ERROR_AT(SUBSYSTEM, TYPE, SOURCE, SPAN, MSG) error_at(SUBSYSTEM, TYPE, SOURCE, SPAN, MSG);
#define ERROR_SPLICE_AT(SUBSYSTEM, TYPE, SOURCE, SPAN, MSG, ...) \
    MSG_SPLICE(MSG, __VA_ARGS__)                                 \
    error_at(SUBSYSTEM, TYPE, SOURCE, SPAN, ERR_MSG_BUFFER);     \

#else
#define ERROR(SUBSYSTEM, TYPE, MSG)
#define ERROR_SPLICE(SUBSYSTEM, TYPE, MSG, ...)
#define ERROR_AT(SUBSYSTEM, TYPE, SOURCE, SPAN, MSG)
#define ERROR_SPLICE_AT(SUBSYSTEM, TYPE, SOURCE, SPAN, MSG, ...)
#endif

// global error message buffer, used for splicing
#define ERR_MSG_BUFFER_LEN 1024
extern char ERR_MSG_BUFFER[ERR_MSG_BUFFER_LEN];
#define MSG_SPLICE(MSG, ...) sprintf(ERR_MSG_BUFFER, MSG, __VA_ARGS__);

void error(err_subsystem_t subsystem, error_type_t type, const char *msg);
void error_at(err_subsystem_t subsystem, error_type_t type, source_t source, span_t span, const char *msg);
void error_highlight_singleline(source_t source, span_t span);
void error_highlight_multiline(source_t source, span_t span);
uint32_t error_output_line(source_t source, uint64_t *idx, uint64_t *line);
void error_output_squiggle_bottom(uint32_t offset, uint32_t start, uint32_t len, bool startWithArrow);
void error_output_squiggle_top(uint32_t offset, uint32_t start, uint32_t len, bool endWithArrow);

#endif //ERROR_H
