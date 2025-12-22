#include <stdio.h>
#include "../backend/veebee.h"
#include "Lexer/source.h"

int main(void) {
    source_t source = SOURCE_Init_FromFile("test.vbee");
    printf("%s", source.buffer);
    return 0;
}
