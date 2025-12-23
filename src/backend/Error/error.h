//
// Created by ds on 12/21/25.
//

#ifndef ERROR_H
#define ERROR_H

// ---------------------------------------------------------------------------------------------------------------------
// Define all VeeBee subsystems
# define FOREACH_SUBSYSTEM(GEN) \
    GEN(SUB_LEXER)              \

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
# define FOREACH_ERROR_TYPE(GEN)     \
    GEN(ERR_INTERNAL)                \
    GEN(ERR_LX_SOURCE_TEXT_NULL)     \
    GEN(ERR_LX_PATH_NULL)            \
    GEN(ERR_LX_PATH_TOO_LONG)        \
    GEN(ERR_LX_FILE_NOT_FOUND)       \
    GEN(ERR_LX_TOKEN_VALUE_TOO_LONG) \
    GEN(ERR_LX_IDENTIFIER_TOO_LONG)  \
    GEN(ERR_LX_UNTERMINATED_STRING)  \
    GEN(ERR_LX_UNEXPECTED_CHARACTER) \

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
#define ERROR_AT(SUBSYSTEM, TYPE, SOURCE, SPAN, MSG) error_at(SUBSYSTEM, TYPE, SOURCE, SPAN, MSG);
#else
#define ERROR(A,B,C)
#define ERROR_AT(A,B,C,D,E)
#endif

void error(err_subsystem_t subsystem, error_type_t type, const char *msg);
void error_at(err_subsystem_t subsystem, error_type_t type, source_t source, span_t span, const char *msg);
void error_highlight_singleline(source_t source, span_t span);
void error_highlight_multiline(source_t source, span_t span);
uint32_t error_output_line(source_t source, uint64_t *idx, uint64_t *line);
void error_output_squiggle_bottom(uint32_t offset, uint32_t start, uint32_t len, bool startWithArrow);
void error_output_squiggle_top(uint32_t offset, uint32_t start, uint32_t len, bool endWithArrow);

#endif //ERROR_H
