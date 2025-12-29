//
// Created by ds on 12/29/25.
//

#ifndef CLI_H
#define CLI_H

const char *CLI_Version = "0.1";

// ---------------------------------------------------------------------------------------------------------------------
// All possible state flags
extern bool CLI_PrintDebugInfo;
extern bool CLI_PrintHelp;
extern bool CLI_PrintVersion;
extern char* CLI_OutputFile;

#define MAX_SOURCE_FILES 1
extern char *CLI_SourceFiles[MAX_SOURCE_FILES]; // max 256 source files allowed
extern int CLI_NumSourceFiles;

// has this gone to shit?
extern bool CLI_HasErrors;
// ---------------------------------------------------------------------------------------------------------------------
// All things cli

void CLI_processFlags(int argc, char **argv);
void CLI_printBanner();
void CLI_printVersion();
void CLI_printHelp();
int CLI_doBusiness();

#endif //CLI_H
