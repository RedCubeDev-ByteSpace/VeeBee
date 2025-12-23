//
// Created by ds on 12/21/25.
//

#ifndef SOURCE_H
#define SOURCE_H
#include <stdint.h>

// ---------------------------------------------------------------------------------------------------------------------
// Source code source types
#define FOREACH_SOURCE_TYPE(GEN)   \
    GEN(SC_EMPTY)                  \
    GEN(SC_FILE)                   \
    GEN(SC_TEXT)                   \

#define GEN_ENUM(ENUM) ENUM,
#define GEN_STRING(STRING) #STRING,

// the actual enum for all our types
typedef enum SOURCE_TYPE {
    FOREACH_SOURCE_TYPE(GEN_ENUM)
} source_type_t;

// names for all these types for debug output
static const char *SOURCE_TYPE_STRING[] = {
    FOREACH_SOURCE_TYPE(GEN_STRING)
};

// ---------------------------------------------------------------------------------------------------------------------
// Source code source
typedef struct SOURCE {
    source_type_t type;
    char filename[256];

    char *buffer;
    uint32_t length;
} source_t;

// methods
source_t SOURCE_Init_Empty();
source_t SOURCE_Init_FromText(char *text);
source_t SOURCE_Init_FromFile(char *filename);
void SOURCE_Unload(source_t *me);

#endif //SOURCE_H
