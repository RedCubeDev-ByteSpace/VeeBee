//
// Created by ds on 12/21/25.
//
#include <stdio.h>
#include "error.h"

#include <string.h>

#include "Debug/ansi.h"

void error(err_subsystem_t subsystem, error_type_t type, const char *msg) {
    printf(RED "[" BRED "%s" RED "](" BRED "%s" RED "): %s\n\n" CRESET, SUBSYSTEM_NAMES[subsystem], ERROR_TYPE_NAMES[type], msg);
}

void error_at(err_subsystem_t subsystem, error_type_t type, source_t source, span_t span, const char *msg) {
    printf(RED "\n[" BRED "%s" RED "](" BRED "%s" RED "): %s\n" CRESET, SUBSYSTEM_NAMES[subsystem], ERROR_TYPE_NAMES[type], msg);

    if (span.start.line == span.end.line) {
        error_highlight_singleline(source, span);
    }
    else {
        error_highlight_multiline(source, span);
    }
}

void error_highlight_singleline(source_t source, span_t span) {
    uint64_t idx = 0;
    uint64_t line = 1;

    // search for the specific line in the buffer
    while (idx < source.length && line < span.start.line) {
        if (source.buffer[idx] == '\n') line++;
        idx++;
    }

    // print the offending line
    uint32_t offsetX = error_output_line(source, &idx, &line);

    // print out the error squiggles
    error_output_squiggle_bottom(offsetX, span.start.column, span.end.column - span.start.column, true);
}

void error_highlight_multiline(source_t source, span_t span) {
    uint64_t idx = 0;
    uint64_t line = 1;

    // search for the first line of our error in the buffer
    while (idx < source.length && line < span.start.line) {
        if (source.buffer[idx] == '\n') line++;
        idx++;
    }

    uint64_t preLineIdx = idx;

    // measure the line length
    while (idx < source.length) {
        if (source.buffer[idx] == '\n') break;
        idx++;
    }

    // measure the line number
    char lineNumber[10];
    sprintf(lineNumber, "%lu | ", line);

    // output the top squiggles
    error_output_squiggle_top(strlen(lineNumber), span.start.column, idx - preLineIdx - span.start.column, false);

    // output all lines from start to finish
    idx = preLineIdx;
    uint32_t offsetX = 0;
    for (int i = 0; i <= span.end.line - span.start.line; ++i) {
        offsetX = error_output_line(source, &idx, &line);
    }

    // output the bottom squiggles
    error_output_squiggle_bottom(offsetX, 0, span.end.column, false);
}

uint32_t error_output_line(source_t source, uint64_t *idx, uint64_t *line) {

    // print out the line number first
    char lineNumber[10];

    // concat the line number into a formatted string
    sprintf(lineNumber, "%lu | ", *line);

    // output the number into the console
    printf(CRESET "%s", lineNumber);

    // output the line into the console
    while (*idx < source.length) {

        // have we hit the end of this line?
        if (source.buffer[*idx] == '\n') {

            // yes -> increase the line counter and get out of here
            (*line)++;
            (*idx)++;
            break;
        }

        // output tabs as single spaces for ease of computing squiggles
        if (source.buffer[*idx] == '\t') putchar(' ');

        // any other chars will be printed normally
        else putchar(source.buffer[*idx]);

        (*idx)++;
    }

    // and the line with a newline char
    putchar('\n');

    // return the length of our printed line number for offset reasons
    return strlen(lineNumber);
}

void error_output_squiggle_bottom(uint32_t offset, uint32_t start, uint32_t len, bool startWithArrow) {
    printf(BRED);

    for (int i = 0; i < offset + start; ++i) {
        putchar(' ');
    }

    // put an arrow at the start of our squiggle
    if (startWithArrow) putchar('^');

    // squiggle as much as we need to
    if (len > (startWithArrow ? 2 : 1))
    for (int i = 0; i < (startWithArrow ? len-2 : len-1); ++i) {
        putchar('~');
    }

    // put an arrow at the end of our squiggle
    if (len >= 2) putchar('^');

    printf(CRESET "\n");
}

void error_output_squiggle_top(uint32_t offset, uint32_t start, uint32_t len, bool endWithArrow) {
    printf(BRED);

    for (int i = 0; i < offset + start; ++i) {
        putchar(' ');
    }

    // put an arrow at the start of our squiggle
    putchar('v');

    // squiggle as much as we need to
    if (len > (endWithArrow ? 2 : 1))
        for (int i = 0; i < (endWithArrow ? len-2 : len-1); ++i) {
            putchar('~');
        }

    // put an arrow at the end of our squiggle
    if (endWithArrow && len >= 2) putchar('v');

    printf(CRESET "\n");
}