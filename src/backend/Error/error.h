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
# define FOREACH_ERROR_TYPE(GEN) \
    GEN(ERR_INTERNAL)            \
    GEN(ERR_LX_SOURCE_TEXT_NULL) \
    GEN(ERR_LX_PATH_NULL)        \
    GEN(ERR_LX_PATH_TOO_LONG)    \
    GEN(ERR_LX_FILE_NOT_FOUND)   \

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
#else
#define ERROR(A,B,C)
#endif

void error(err_subsystem_t subsystem, error_type_t type, const char *msg);

#endif //ERROR_H
