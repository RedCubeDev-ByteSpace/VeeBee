#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../backend/veebee.h"
#include "../backend/Debug/ansi.h"
#include "cli.h"
#include "Binder/binder.h"
#include "Binder/indexer.h"
#include "Binder/symbol_resolver.h"
#include "Debug/pretty_print.h"
#include "Error/error.h"
#include "Lexer/lexer.h"
#include "Parser/parser.h"

// ---------------------------------------------------------------------------------------------------------------------
// define all static variables
bool CLI_PrintDebugInfo       = false;
bool CLI_PrintHelp            = false;
bool CLI_PrintVersion         = false;
char *CLI_OutputFile          = NULL;
char *CLI_SourceFiles[MAX_SOURCE_FILES];
int CLI_NumSourceFiles        = 0;
bool CLI_HasErrors            = false;

// ---------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv) {

    // output the little veebee header
    CLI_printBanner();

    // go through the arguments
    CLI_processFlags(argc, argv);

    // stop here if there have already been errors
    if (CLI_HasErrors) return 1;

    // was the help page requested? or zero arguments given?
    if (CLI_PrintHelp || argc == 1) {
        CLI_printHelp();
        return 0;
    }

    // was the version page requested?
    if (CLI_PrintVersion) {
        CLI_printVersion();
        return 0;
    }

    // if we've reached this point - do compilation and runtime things
    return CLI_doBusiness();
}

// ---------------------------------------------------------------------------------------------------------------------

void CLI_processFlags(int argc, char **argv) {
    int idx = 1;

    // step through the arg list
    while (idx < argc) {

        // -------------------------------------------------------------------------------------------------------------
        // -h: print the help page, stops execution
        if (strcmp(argv[idx], "-h") == 0) {
            CLI_PrintHelp = true;

            idx++;
            continue;
        }


        // -------------------------------------------------------------------------------------------------------------
        // -v: print the cli and veebee versions, stops execution
        if (strcmp(argv[idx], "-v") == 0) {
            CLI_PrintVersion = true;

            idx++;
            continue;
        }

        // -------------------------------------------------------------------------------------------------------------
        // -d: print debug info like token lists, parse trees, etc
        if (strcmp(argv[idx], "-d") == 0) {
            CLI_PrintDebugInfo = true;

            idx++;
            continue;
        }

        // -------------------------------------------------------------------------------------------------------------
        // -c: compile to a bytecode file, does not execute the bytecode
        // - requires an argument
        if (strcmp(argv[idx], "-c") == 0 && idx < argc-1) {
            CLI_OutputFile = argv[idx+1];

            idx += 2;
            continue;
        }

        // -------------------------------------------------------------------------------------------------------------
        // anything not prefixed with a flag will be assumed to be a source file
        if (CLI_NumSourceFiles < MAX_SOURCE_FILES) {
            CLI_SourceFiles[CLI_NumSourceFiles++] = argv[idx];
            idx++;
        } else {
            ERROR(SUB_CLI, ERR_CL_TOO_MANY_SOURCES, "The commandline was given more source files than allowed")
            CLI_HasErrors = true;
            break;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void CLI_printBanner() {

    printf(MAG);

    printf(" _____         _____         \n");
    printf("|  |  |___ ___| __  |___ ___ \n");
    printf("|  |  | -_| -_| __ -| -_| -_|\n");
    printf(" \\___/|___|___|_____|___|___|\n");
    printf(CRESET "--+ VeeBee - by Bytespace +--\n");
}

void CLI_printVersion() {
    printf("\n");
    printf("VeeBee CLI Version: %s\n", CLI_Version);
    printf("VeeBee Version: %s\n", VEEBEE_Version);
}

void CLI_printHelp() {
    printf("\n");
    printf("Welcome to the VeeBee CLI help page!\n");
    printf("General Usage: ./veebee <source files> [options]\n\n");
    printf(MAG  "<file>    " CRESET "| Any non flag will be treated as a VeeBee source file\n");
    printf(BMAG "-h        " CRESET "| Displays this help page\n");
    printf(BMAG "-v        " CRESET "| Displays the current CLI and VeeBee version\n");
    printf(BMAG "-c <file> " CRESET "| Compile the given sources to a .cee byte code file\n");

    printf("\nThere are also a few developer goodies, if you're a nerd:\n");
    printf(BMAG "-d        " CRESET "| Output debug information like tokens and parse trees\n");
    printf(BMAG "-p <file> " CRESET "| Print the contents of a .cee byte code file in a human readable form\n");
}

// ---------------------------------------------------------------------------------------------------------------------

int CLI_doBusiness() {
    // do we even have any source files
    if (CLI_NumSourceFiles == 0) {
        ERROR(SUB_CLI, ERR_CL_NO_SOURCES_GIVEN, "The program was run without any source files")
        return 1;
    }

    // make sure all source files exist
    for (int i = 0; i < CLI_NumSourceFiles; ++i) {
        if (access(CLI_SourceFiles[i], F_OK) != 0) {
            ERROR(SUB_CLI, ERR_CL_SOURCE_DOESNT_EXIST, "One or more of the given source files doesnt exist")
            return 1;
        }
    }

    if (CLI_PrintDebugInfo) {
        printf("\nList of sources: \n");
        for (int i = 0; i < CLI_NumSourceFiles; ++i) {
            printf("- \"%s\"\n", CLI_SourceFiles[i]);
        }
    }

    // load the source
    source_t source = SOURCE_Init_FromFile(CLI_SourceFiles[0]);

    // lex the input
    lexer_t *lexer = LEXER_Init(source);
    LEXER_Lex(lexer);

    // if debug output is enabled, output the token list
    if (CLI_PrintDebugInfo) {
        printf("\n");
        DBG_PRETTY_PRINT_Print_TokenList(lexer->tokens);
        //printf("\n");
        //DBG_PRETTY_PRINT_Print_TokenList_AsSource(lexer->tokens);
    }

    // parse!
    parser_t *parser = PARSER_Init(source, &lexer->tokens);
    PARSER_Parse(parser);

    // have there been errors?
    if (parser->hasError) {
        return 1;
    }

    if (CLI_PrintDebugInfo) {
        printf("\nList of parsed members:\n");
        DBG_INIT_INDENT();
        DBG_PRETTY_PRINT_Print_LSAstNode_List(parser->lsMembers);
    }

    // create a new binder
    binder_t *binder = BINDER_Init();

    // create an index of all module and type names
    BINDER_CreateModuleIndex(binder, source, parser->lsMembers);
    if (binder->hasError) goto unload;

    // create an index of all type fields
    BINDER_CreateTypeIndex(binder, (module_symbol_t*)binder->programUnit->lsModules.symbols[0]);
    if (binder->hasError) goto unload;

    // create an index of all functions and subroutines
    BINDER_CreateProcedureIndex(binder, (module_symbol_t*)binder->programUnit->lsModules.symbols[0]);
    if (binder->hasError) goto unload;

    // now that we know all the procedures that exist in this program we can initialize the global procedure buffer
    BD_PROGRAM_UNIT_InitializeProcedureBuffer(binder->programUnit);


    // -----------------------------------------------------------------------------------------------------------------
    // create a c runtime module

    module_symbol_t *myModule = malloc(sizeof(module_symbol_t));
    myModule->base.type = MODULE_SYMBOL;
    myModule->isExternal = true;
    myModule->lsProcedures = BD_SYMBOL_LIST_Init();
    myModule->lsTypes = BD_SYMBOL_LIST_Init();
    strcpy(myModule->base.name, "runtime");

    // Add a print function
    procedure_symbol_t *prcPrint = malloc(sizeof(procedure_symbol_t));
    prcPrint->base.type = PROCEDURE_SYMBOL;
    strcpy(prcPrint->base.name, "print");

    prcPrint->procedureId = 1 << 31;
    prcPrint->visibility = PUBLIC;
    prcPrint->lsParameters = BD_SYMBOL_LIST_Init();

    parameter_symbol_t *prcPrintMsg = malloc(sizeof(parameter_symbol_t));
    prcPrintMsg->base.type = PARAMETER_SYMBOL;
    strcpy(prcPrintMsg->base.name, "msg");
    prcPrintMsg->isOptional = false;
    prcPrintMsg->passingType = PASS_BY_REFERENCE;
    prcPrintMsg->exprDefaultValue = NULL;
    prcPrintMsg->symType = BINDER_resolveTypeNameFromBuffer(binder, "string");

    BD_SYMBOL_LIST_Add(&prcPrint->lsParameters, (symbol_t*)prcPrintMsg);
    BD_SYMBOL_LIST_Add(&myModule->lsProcedures, (symbol_t*)prcPrint);
    BD_SYMBOL_LIST_Add(&binder->programUnit->lsModules, (symbol_t*)myModule);





    // and then finally bind all our procedure bodies!
    BINDER_BindProcedureBodies(binder);
    if (binder->hasError) goto unload;

    DBG_PRETTY_PRINT_Print_ProgramUnit(binder->programUnit);

    // unload everything
unload:
    BD_PROGRAM_UNIT_Unload(binder->programUnit);
    BINDER_Unload(binder);
    PS_LS_AST_NODE_LIST_Unload(parser->lsMembers);
    LX_TOKEN_LIST_Unload(lexer->tokens);
    PARSER_Unload(parser);
    LEXER_Unload(lexer);
    SOURCE_Unload(source);

    return 0;
}