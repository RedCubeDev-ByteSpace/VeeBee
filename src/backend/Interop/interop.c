//
// Created by ds on 2/10/26.
//
#include "interop.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "Binder/symbol_resolver.h"
#include "Error/error.h"

type_symbol_t *INTEROP_LookupType(program_unit_t *prgUnit, char *name) {
    // -----------------------------------------------------------------------------------------------------------------
    // check if this is a builtin symbol
    for (int i = 0; i < prgUnit->lsBuiltinTypes.length; i++) {
        if (strcmp(prgUnit->lsBuiltinTypes.symbols[i]->name, name) == 0) {

            // found it!
            return (type_symbol_t*)prgUnit->lsBuiltinTypes.symbols[i];
        }
    }

    // anything else is not available at compile time
    ERROR_SPLICE(SUB_INTEROP, ERR_IO_UNKNOWN_TYPE_NAME, "The type name '%s' could not be resolved to a builtin type", name);

    return NULL;
}

module_symbol_t *INTEROP_CreateModule(program_unit_t *prgUnit, char *moduleName) {

    // Allocate a new symbol
    module_symbol_t *myModule = malloc(sizeof(module_symbol_t));

    // Set it up as an external module
    myModule->base.type = MODULE_SYMBOL;
    myModule->isExternal = true;
    myModule->lsProcedures = BD_SYMBOL_LIST_Init();
    myModule->lsTypes = BD_SYMBOL_LIST_Init();

    // copy over the symbol name in lowercase
    for (int i = 0; i < strlen(moduleName); ++i) {
        myModule->base.name[i] = (char)tolower(moduleName[i]);
    }
    myModule->base.name[strlen(moduleName)] = 0;

    // register it
    BD_SYMBOL_LIST_Add(&prgUnit->lsModules, (symbol_t*)myModule);

    // done
    return myModule;
}

procedure_symbol_t *INTEROP_CreateProcedure(program_unit_t *prgUnit, module_symbol_t *module, char *procName, type_symbol_t *returnType) {

    // Allocate a new symbol
    procedure_symbol_t *myProc = malloc(sizeof(procedure_symbol_t));

    // set up the symbol
    myProc->base.type = PROCEDURE_SYMBOL;

    // copy over the symbol name in lowercase
    for (int i = 0; i < strlen(procName); ++i) {
        myProc->base.name[i] = (char)tolower(procName[i]);
    }
    myProc->base.name[strlen(procName)] = 0;

    // set the parameters
    myProc->procedureId = 1 << 31 | prgUnit->externalProcedureCounter;
    myProc->visibility = PUBLIC;
    myProc->lsParameters = BD_SYMBOL_LIST_Init();
    myProc->lsLabels = BD_SYMBOL_LIST_Init();
    myProc->lsBuckets = BD_SYMBOL_LIST_Init();
    myProc->isVariadic = false;
    myProc->symReturnType = returnType;

    // count this procedure
    prgUnit->externalProcedureCounter++;

    // add it to the module
    BD_SYMBOL_LIST_Add(&module->lsProcedures, (symbol_t*)myProc);

    // done
    return myProc;
}

void INTEROP_AddParameter(procedure_symbol_t *procedure, char *parameterName, type_symbol_t *parameterType, passing_type_t passingType, bool isOptional) {

    // Allocate a new symbol
    parameter_symbol_t *myParam = malloc(sizeof(parameter_symbol_t));

    // set up the symbol basics
    myParam->base.type = PARAMETER_SYMBOL;

    // copy over the symbol name in lowercase
    for (int i = 0; i < strlen(parameterName); ++i) {
        myParam->base.name[i] = (char)tolower(parameterName[i]);
    }
    myParam->base.name[strlen(parameterName)] = 0;

    // configure the parameter options
    myParam->isOptional = isOptional;
    myParam->passingType = passingType;
    myParam->exprDefaultValue = NULL;
    myParam->symType = parameterType;

    // add this parameter to the procedure
    BD_SYMBOL_LIST_Add(&procedure->lsParameters, (symbol_t*)myParam);
}