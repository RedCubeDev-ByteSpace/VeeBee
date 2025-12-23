//
// Created by ds on 12/21/25.
//
#include <string.h>
#include "source.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Error/error.h"

source_t SOURCE_Init_Empty() {
    // fallback for when things dont work out
    source_t source;

    // mark this as an empty source
    source.type = SC_EMPTY;

    // set inoffensive placeholder values
    source.buffer = malloc(1);
    source.buffer[0] = 0;
    source.length = 0;

    // theres no filename attached to this
    source.filename[0] = 0;

    // done
    return source;
}

source_t SOURCE_Init_FromText(char *text) {
    // create a new source to store all this in
    source_t source;

    // theres no filename for this source as its just created from a string
    source.type = SC_TEXT;
    source.filename[0] = 0;

    // is the source text valid?
    if (text == NULL) {

        ERROR(SUB_LEXER, ERR_LX_SOURCE_TEXT_NULL, "The given input text was null");

        // return an empty source as an error fallback
        return SOURCE_Init_Empty();
    }

    // get the length of our input source
    uint64_t len = strlen(text);

    // copy the text into a new buffer
    source.length = len;
    source.buffer = malloc(len + 1);
    strcpy(source.buffer, text);

    // done
    return source;
}

source_t SOURCE_Init_FromFile(char *filename) {
    // create a new source to store all this in
    source_t source;

    // check if the filename is a valid string
    if (filename == NULL) {

        // no sir
        ERROR(SUB_LEXER, ERR_LX_PATH_NULL, "The given input file path is null");

        // return an empty source as an error fallback
        return SOURCE_Init_Empty();
    }

    // check if the filename is too long
    uint64_t len = strlen(filename);
    if (len > 255) {

        // looks like it is
        ERROR(SUB_LEXER, ERR_LX_PATH_TOO_LONG, "The given input file path is longer than the allowed 255 chars");

        // return an empty source as an error fallback
        return SOURCE_Init_Empty();
    }

    // does the file exist?
    if (access(filename, F_OK) != 0) {

        // nope, it doesnt
        ERROR(SUB_LEXER, ERR_LX_FILE_NOT_FOUND, "The given input file was not found");

        // return an empty source as an error fallback
        return SOURCE_Init_Empty();
    }

    // store the given filename
    source.type = SC_FILE;
    strcpy(source.filename, filename);

    // read the source file
    FILE *file = fopen(filename, "r");
    char buffer[100];

    // initialize the destination buffer
    source.buffer = NULL;
    source.length = 0;

    while(fgets(buffer, 100, file)) {
        // get the amount of chars we read
        uint32_t bufferLength = strlen(buffer);

        // reallocate the source buffer
        source.buffer = realloc(source.buffer, source.length + bufferLength + 1);

        // copy it over
        strcpy(source.buffer + source.length, buffer);

        // add this length to the source buffers length
        source.length += bufferLength;
    }

    // were done here
    fclose(file);

    // done
    return source;
}

void SOURCE_Unload(source_t *me) {
    // free the source buffer
    free(me->buffer);
}