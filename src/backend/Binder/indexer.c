//
// Created by ds on 1/29/26.
//
#include <stddef.h>
#include "indexer.h"
#include "binder.h"

#include <stdlib.h>
#include <string.h>

#include "symbol_resolver.h"
#include "AST/Tight/Symbols/local_variable_symbol.h"
#include "AST/Tight/Symbols/type_field_symbol.h"
#include "AST/Loose/Clauses/type_field_clause.h"
#include "AST/Loose/Members/type_member.h"
#include "AST/Loose/Statements/label_statement.h"
#include "Error/error.h"
#include "Parser/parser.h"


// ---------------------------------------------------------------------------------------------------------------------
// BINDER_CreateModuleIndex:
// go over all parsed members, configure this module, and create symbols for all types
// This is the first pass of indexing, it establishes all type names in all modules
void BINDER_CreateModuleIndex(binder_t *me, source_t source, ls_ast_node_list_t lsMembers) {

    // create a new program unit, it will contain all our modules
    module_symbol_t *newModule = NULL;

    // see if we have a Module member at the beginning of the file
    if (lsMembers.length > 0 && lsMembers.nodes[0]->type == LS_MODULE_MEMBER) {
        newModule = BINDER_indexModule((ls_module_member_node_t*)lsMembers.nodes[0]);
    }

    // if not, create a default module
    else {

        // allocate a new module symbol
        newModule = malloc(sizeof(module_symbol_t));
        newModule->base.type = MODULE_SYMBOL;

        // set the name to the default
        strcpy(newModule->base.name, BD_DEFAULT_MODULE_NAME);

        // this is an automatically inserted module
        newModule->isAutoInserted = true;

        // this is also a veebee module
        newModule->isExternal = false;

        // initialize all lists
        newModule->lsProcedures = BD_SYMBOL_LIST_Init();
        newModule->lsTypes = BD_SYMBOL_LIST_Init();
    }

    // set the source file
    newModule->source = source;
    newModule->lsMembers = lsMembers;

    // -----------------------------------------------------------------------------------------------------------------
    // now that we have a module -> iterate through all members and index them

    // if the module was defined by a module member -> skip the first member
    int startIndex = newModule->isAutoInserted ? 0 : 1;
    for (int i = startIndex; i < lsMembers.length; ++i) {

        // if we found a module member -> throw an error
        if (lsMembers.nodes[i]->type == LS_MODULE_MEMBER) {
            ls_module_member_node_t *memberNode = (ls_module_member_node_t*)lsMembers.nodes[i];

            // did we get multiple module members?
            if (!newModule->isAutoInserted) {
                ERROR_AT(SUB_BINDER, ERR_BD_MORE_THAN_ONE_MODULE_MEMBERS, newModule->source, SPAN_FromToken(*memberNode->kwModule), "Only one module definition is allowed per file")
                me->hasError = true;
                break;
            }

            // is this module member at the wrong position
            ERROR_AT(SUB_BINDER, ERR_BD_MODULE_MEMBER_NOT_AT_BEGINNING_OF_FILE, newModule->source, SPAN_FromToken(*memberNode->kwModule), "The module definition must be located at the beginning of the file")
            me->hasError = true;
            break;
        }

        // if we found a type member -> declare its name as a type
        if (lsMembers.nodes[i]->type == LS_TYPE_MEMBER) {
            ls_type_member_node_t *memberNode = (ls_type_member_node_t*)lsMembers.nodes[i];
            BINDER_addSymbolToModuleSymbol(me, newModule, (symbol_t*)BINDER_indexTypeName(memberNode, i));
            if (me->hasError) break;
        }
    }

    // try to add this module symbol to the program unit
    BINDER_addModuleToProgramUnit(me, me->programUnit, newModule);
}

// ---------------------------------------------------------------------------------------------------------------------
// BINDER_CreateTypeIndex:
// go over all previously found types, index their fields and add them to the type symbol
// This is the second pass of indexing, it establishes all fields of user defined types
void BINDER_CreateTypeIndex(binder_t *me, module_symbol_t *symModule) {

    for (int i = 0; i < symModule->lsTypes.length; ++i) {

        BINDER_indexTypeFields(me, symModule, (type_symbol_t*)symModule->lsTypes.symbols[i]);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// BINDER_CreateProcedureIndex:
// go over all members of this module, index and create function and subroutine symbols
void BINDER_CreateProcedureIndex(binder_t *me, module_symbol_t *symModule) {

    // look through all members
    for (int i = 0; i < symModule->lsMembers.length; ++i) {

        // is this a function declaration member?
        if (symModule->lsMembers.nodes[i]->type == LS_FUNCTION_MEMBER) {
            ls_function_member_node_t *memberNode = (ls_function_member_node_t*)symModule->lsMembers.nodes[i];
            BINDER_addSymbolToModuleSymbol(me, symModule, (symbol_t*)BINDER_indexFunction(me, symModule, memberNode, i));
            if (me->hasError) break;
        }

        // is this a subroutine declaration member?
        if (symModule->lsMembers.nodes[i]->type == LS_SUBROUTINE_MEMBER) {
            ls_sub_member_node_t *memberNode = (ls_sub_member_node_t*)symModule->lsMembers.nodes[i];
            BINDER_addSymbolToModuleSymbol(me, symModule, (symbol_t*)BINDER_indexSubroutine(me, symModule, memberNode, i));
            if (me->hasError) break;
        }
    }

}

// ---------------------------------------------------------------------------------------------------------------------
module_symbol_t *BINDER_indexModule(ls_module_member_node_t *memModule) {
    module_symbol_t *newModule = malloc(sizeof(module_symbol_t));
    newModule->base.type = MODULE_SYMBOL;

    // copy over the module name
    strcpy(newModule->base.name, memModule->idModuleName->strValue);

    // this is a user defined module
    newModule->isAutoInserted = false;

    // this is also a veebee module
    newModule->isExternal = false;

    // remember where this was declared
    newModule->base.declarationSpan = SPAN_FromToken(*memModule->idModuleName);

    // initialize all lists
    newModule->lsProcedures = BD_SYMBOL_LIST_Init();
    newModule->lsTypes = BD_SYMBOL_LIST_Init();

    return newModule;
}

type_symbol_t *BINDER_indexTypeName(ls_type_member_node_t *memType, int idx) {
    type_symbol_t *newType = malloc(sizeof(type_symbol_t));
    newType->base.type = TYPE_SYMBOL;

    // copy over the type name
    strcpy(newType->base.name, memType->idTypeName->strValue);

    // mark this as a user defined type
    newType->typeOfType = TYPE_USER_DEFINED;

    // remember where this was declared
    newType->base.declarationSpan = SPAN_FromToken(*memType->idTypeName);

    // initialize the field list
    newType->lsFields = BD_SYMBOL_LIST_Init();

    // remember the index of the member defining this type for later
    newType->idxTypeMember = idx;

    return newType;
}

void BINDER_indexTypeFields(binder_t *me, module_symbol_t *symModule, type_symbol_t *symType) {

    // look up the definition of this type
    ls_type_member_node_t *definition = (ls_type_member_node_t*)symModule->lsMembers.nodes[symType->idxTypeMember];

    // does this definition have at least one field?
    if (definition->lsFields.length == 0) {
        ERROR_AT(SUB_BINDER, ERR_BD_NO_FIELDS_IN_TYPE, symModule->source, SPAN_FromToken(*definition->idTypeName), "A user defined type must include at least one field");
        me->hasError = true;
        return;
    }

    // go through all its fields and create field symbols for them
    for (int i = 0; i < definition->lsFields.length; ++i) {

        ls_type_field_clause_node_t *clsField = (ls_type_field_clause_node_t*)definition->lsFields.nodes[i];

        // look up the fields type
        type_symbol_t *fieldType = BINDER_ResolveAsClause(me, symModule, clsField->clsType, BD_RS_ALLOW_ONLY_GENERIC_SIZE_ARRAYS);
        if (me->hasError) return;

        // create a new field symbol
        type_field_symbol_t *symField = malloc(sizeof(type_field_symbol_t));
        symField->base.type = TYPE_FIELD_SYMBOL;

        // copy the field name over
        strcpy(symField->base.name, clsField->idFieldName->strValue);

        // assign the type
        symField->symType = fieldType;

        // remember where this was declared
        symField->base.declarationSpan = SPAN_FromToken(*clsField->idFieldName);

        // add it to our user defined type symbol
        if (!BINDER_addFieldToTypeSymbol(me, symModule, symType, symField)) {
            break;
        }
    }

}

procedure_symbol_t *BINDER_indexFunction(binder_t *me, module_symbol_t *symModule, ls_function_member_node_t *memFunction, int idx) {

    // what is the visibility of this function
    procedure_visibility_t visibility = PUBLIC;
    if (memFunction->kwPublic != NULL) {
        visibility = PUBLIC;
    }
    else if (memFunction->kwPrivate != NULL) {
        visibility = PRIVATE;
    }
    else if (memFunction->kwFriend != NULL) {
        visibility = FRIEND;
    }

    // if the visibility is friend, are we allowed to use it in this context?
    if (visibility == FRIEND) {
        ERROR_AT(SUB_BINDER, ERR_BD_FRIEND_MODIFIER_NOT_ALLOWED, symModule->source, SPAN_FromToken(*memFunction->kwFriend), "The Friend modifier is only allowed to be used inside class modules");
        me->hasError = true;
        return NULL;
    }

    // is there a return type given?
    type_symbol_t *returnType = (type_symbol_t*)me->programUnit->lsBuiltinTypes.symbols[IDX_BUILTIN_VARIANT];
    if (memFunction->clsReturnType != NULL) {

        // look up the return type
        returnType = BINDER_ResolveAsClause(me, symModule, memFunction->clsReturnType, BD_RS_NO_ARRAYS);
        if (me->hasError) return NULL;
    }

    // Validate the function parameters to make sure theres not an illegal combination of param modifiers
    if (!BINDER_validateProcedureParameters(me, symModule, memFunction->lsParameters)) {
        return NULL; // somethings wrong here
    }

    // -----------------------------------------------------------------------------------------------------------------
    // allocate a new symbol for this function
    procedure_symbol_t *newFunction = malloc(sizeof(procedure_symbol_t));
    newFunction->base.type = PROCEDURE_SYMBOL;

    // copy over its name
    strcpy(newFunction->base.name, memFunction->idName->strValue);

    // remember where this was declared
    newFunction->base.declarationSpan = SPAN_FromToken(*memFunction->idName);

    // initialize the parameter list
    newFunction->lsParameters = BD_SYMBOL_LIST_Init();

    // initialize the bucket list
    newFunction->lsBuckets = BD_SYMBOL_LIST_Init();

    // initialize the label list
    newFunction->lsLabels = BD_SYMBOL_LIST_Init();

    // remember which member this function symbol belongs to
    newFunction->idxMember = idx;

    // get this procedures proc index
    newFunction->procedureId = me->programUnit->procedureCounter++;

    // generate a return label
    newFunction->returnLabel = BINDER_generateLabel(me, symModule, newFunction, SPAN_FromToken(*memFunction->idName), "end_of_procedure");

    // -----------------------------------------------------------------------------------------------------------------
    // remember the visibility
    newFunction->visibility = visibility;

    // -----------------------------------------------------------------------------------------------------------------
    // remember the return type
    newFunction->symReturnType = returnType;

    // set up a local variable for the return value of this function, this is always bucket number 0
    local_variable_symbol_t *returnVariable = malloc(sizeof(local_variable_symbol_t));

    // setup the symbol basics
    returnVariable->base.type = LOCAL_VARIABLE_SYMBOL;
    strcpy(returnVariable->base.name, memFunction->idName->strValue);
    returnVariable->base.declarationSpan = SPAN_FromToken(*memFunction->idName);

    // this is always bucket index zero
    returnVariable->bucketIndex = 0;

    // set it to the same type as the return type
    returnVariable->symType = returnType;

    // add it to the bucket list
    BD_SYMBOL_LIST_Add(&newFunction->lsBuckets, (symbol_t*)returnVariable);

    // -----------------------------------------------------------------------------------------------------------------
    // Bind all parameters
    BINDER_indexProcedureParameters(me, symModule, newFunction, memFunction->lsParameters, 1);

    return newFunction;
}

procedure_symbol_t *BINDER_indexSubroutine(binder_t *me, module_symbol_t *symModule, ls_sub_member_node_t *memSubroutine, int idx) {

    // what is the visibility of this subroutine
    procedure_visibility_t visibility = PUBLIC;
    if (memSubroutine->kwPublic != NULL) {
        visibility = PUBLIC;
    }
    else if (memSubroutine->kwPrivate != NULL) {
        visibility = PRIVATE;
    }
    else if (memSubroutine->kwFriend != NULL) {
        visibility = FRIEND;
    }

    // if the visibility is friend, are we allowed to use it in this context?
    if (visibility == FRIEND) {
        ERROR_AT(SUB_BINDER, ERR_BD_FRIEND_MODIFIER_NOT_ALLOWED, symModule->source, SPAN_FromToken(*memSubroutine->kwFriend), "The Friend modifier is only allowed to be used inside class modules");
        me->hasError = true;
        return NULL;
    }

    // Validate the subroutines parameters to make sure theres not an illegal combination of param modifiers
    if (!BINDER_validateProcedureParameters(me, symModule, memSubroutine->lsParameters)) {
        return NULL; // somethings wrong here
    }

    // -----------------------------------------------------------------------------------------------------------------
    // allocate a new symbol for this subroutine
    procedure_symbol_t *newSubroutine = malloc(sizeof(procedure_symbol_t));
    newSubroutine->base.type = PROCEDURE_SYMBOL;

    // copy over its name
    strcpy(newSubroutine->base.name, memSubroutine->idName->strValue);

    // remember where this was declared
    newSubroutine->base.declarationSpan = SPAN_FromToken(*memSubroutine->idName);

    // initialize the parameter list
    newSubroutine->lsParameters = BD_SYMBOL_LIST_Init();

    // initialize the bucket list
    newSubroutine->lsBuckets = BD_SYMBOL_LIST_Init();

    // initialize the label list
    newSubroutine->lsLabels = BD_SYMBOL_LIST_Init();

    // remember which member this function symbol belongs to
    newSubroutine->idxMember = idx;

    // get this procedures proc index
    newSubroutine->procedureId = me->programUnit->procedureCounter++;

    // generate a return label
    newSubroutine->returnLabel = BINDER_generateLabel(me, symModule, newSubroutine, SPAN_FromToken(*memSubroutine->idName), "end_of_procedure");

    // -----------------------------------------------------------------------------------------------------------------
    // remember the visibility
    newSubroutine->visibility = visibility;

    // because this is a subroutine -> we dont return anything
    newSubroutine->symReturnType = NULL;

    // -----------------------------------------------------------------------------------------------------------------
    // Bind all parameters
    BINDER_indexProcedureParameters(me, symModule, newSubroutine, memSubroutine->lsParameters, 0);

    return newSubroutine;
}
bool BINDER_validateProcedureParameters(binder_t *me, module_symbol_t *symModule, ls_ast_node_list_t lsParams) {

    // -----------------------------------------------------------------------------------------------------------------
    // OPTIONAL PARAMETERS
    int numOptionalParams = 0;
    for (int i = 0; i < lsParams.length; ++i) {
        ls_parameter_clause_node_t *param = (ls_parameter_clause_node_t*)lsParams.nodes[i];

        // is this parameter optional?
        if (param->kwOptional != NULL) {

            // does this non optional parameter have a default value?
            // TODO: implement default values
            if (param->expDefaultValue != NULL) {
                ERROR_AT(SUB_BINDER, ERR_NOT_IMPLEMENTED, symModule->source, SPAN_FromToken(*param->idParamName), "Default values have not been implemented yet");
                me->hasError = true;
                return false;
            }

            // if so -> count it
            numOptionalParams++;
        }

        // if not
        else {

            // check if we already came across optional parameters
            // -> this is illegal as optional parameters must always come last
            if (numOptionalParams > 0) {
                ERROR_AT(SUB_BINDER, ERR_BD_INVALID_NON_OPTIONAL_PARAMETER, symModule->source, SPAN_FromToken(*param->idParamName), "Optional parameters must always be located at the end of the parameter list, an optional parameter was expected");
                me->hasError = true;
                return false;
            }

            // does this non optional parameter have a default value?
            if (param->expDefaultValue != NULL) {
                ERROR_AT(SUB_BINDER, ERR_BD_ILLEGAL_COMBINATION_OF_MODIFIERS, symModule->source, SPAN_FromToken(*param->idParamName), "Only optional parameters are allowed to have a default value");
                me->hasError = true;
                return false;
            }
        }
    }

    // -----------------------------------------------------------------------------------------------------------------
    // PARAM ARRAY PARAMETER
    int numParamArrayParams = 0;
    for (int i = 0; i < lsParams.length; ++i) {
        ls_parameter_clause_node_t *param = (ls_parameter_clause_node_t*)lsParams.nodes[i];

        // is this parameter a param array
        if (param->kwParamArray != NULL) {

            // has there been a param array already?
            if (numParamArrayParams > 0) {
                ERROR_AT(SUB_BINDER, ERR_BD_INVALID_PARAMARRAY_PARAMETER, symModule->source, SPAN_FromToken(*param->idParamName), "A ParamArray parameter has already been defined for this function, only one ParamArray parameter is allowed per function");
                me->hasError = true;
                return false;
            }

            // if so -> count it
            numParamArrayParams++;

            // make sure its not also optional, thats illegal
            if (param->kwOptional != NULL) {
                ERROR_AT(SUB_BINDER, ERR_BD_ILLEGAL_COMBINATION_OF_MODIFIERS, symModule->source, SPAN_FromToken(*param->kwOptional), "ParamArray parameters cannot be optional");
                me->hasError = true;
                return false;
            }

            // also make sure its not marked as byRef, thats illegal
            if (param->kwByRef != NULL) {
                ERROR_AT(SUB_BINDER, ERR_BD_ILLEGAL_COMBINATION_OF_MODIFIERS, symModule->source, SPAN_FromToken(*param->kwByRef), "ParamArray parameters cannot be passed by reference");
                me->hasError = true;
                return false;
            }

            // and also dont allow an explicit byVal, all of this is handled internally, the user does not have any say in it
            if (param->kwByVal != NULL) {
                ERROR_AT(SUB_BINDER, ERR_BD_ILLEGAL_COMBINATION_OF_MODIFIERS, symModule->source, SPAN_FromToken(*param->kwByVal), "ParamArray parameters cannot be passed by value explicitly");
                me->hasError = true;
                return false;
            }
        }

        // is this not a param array
        else {

            // has there already been one?
            // -> thats illegal, a param array must always be at the very end of the param list
            if (numParamArrayParams > 0) {
                ERROR_AT(SUB_BINDER, ERR_BD_INVALID_NON_PARAMARRAY_PARAMETER, symModule->source, SPAN_FromToken(*param->idParamName), "Invalid parameter after a ParamArray parameter, the ParamArray parameter must always be the final parameter of the list");
                me->hasError = true;
                return false;
            }
        }
    }

    // is this function using optionals and param array parameters?
    // that is so illegal
    if (numOptionalParams > 0 && numParamArrayParams > 0) {

        // for the span:
        // get the first parameter id and the last parameter id
        span_t errorSpan = SPAN_Between(
            SPAN_FromToken(*((ls_parameter_clause_node_t*)lsParams.nodes[0])->idParamName),
            SPAN_FromToken(*((ls_parameter_clause_node_t*)lsParams.nodes[lsParams.length-1])->idParamName)
        );

        ERROR_AT(SUB_BINDER, ERR_BD_ILLEGAL_COMBINATION_OF_MODIFIERS, symModule->source, errorSpan, "Function parameters cannot contain both Optional parameters and ParamArray parameters");
        me->hasError = true;
        return false;
    }

    // we passed all the checks! yayy
    return true;
}

void BINDER_indexProcedureParameters(binder_t *me, module_symbol_t *symModule, procedure_symbol_t *symProc, ls_ast_node_list_t lsParams, int startIdx) {

    for (int i = 0; i < lsParams.length; ++i) {
        ls_parameter_clause_node_t *param = (ls_parameter_clause_node_t*)lsParams.nodes[i];

        // resolve the type of this parameter
        type_symbol_t *paramType = BINDER_ResolveAsClause(me, symModule, param->clsType, BD_RS_ALLOW_ONLY_GENERIC_SIZE_ARRAYS);
        if (me->hasError) return;

        // select the type of parameter this is
        passing_type_t passingType = PASS_BY_REFERENCE; // pass by reference as default

        // check if we instead should pass by value
        if (param->kwByVal != NULL) {
            passingType = PASS_BY_VALUE;
        }

        // is this parameter optional?
        bool isOptional = false;
        if (param->kwOptional != NULL) {
            isOptional = true;
        }

        // is this a ParamArray parameter -> mark the procedure as such
        if (param->kwParamArray != NULL) {
            symProc->isVariadic = true;

            // wait, does this parameter have an array type?
            // otherwise it cannot be a ParamArray parameter
            if (paramType->typeOfType != TYPE_ARRAY) {
                ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_UNEXPECTED_NON_ARRAY_TYPE, symModule->source, SPAN_FromToken(*param->kwParamArray), "ParamArray is only allowed to be used on array types, current type: '%s'", paramType->base.name)
                me->hasError = true;
                return;
            }
        }

        // assign a bucket index for data storage
        // parameters always get the first buckets
        int bucketIdx = startIdx + i; // bucket zero is always reserved for the procedures return value, even if it doesnt have one

        // allocate a new symbol for this parameter
        parameter_symbol_t *newParamSymbol = malloc(sizeof(parameter_symbol_t));

        // setup the base symbol
        newParamSymbol->base.type = PARAMETER_SYMBOL;
        strcpy(newParamSymbol->base.name, param->idParamName->strValue);
        newParamSymbol->base.declarationSpan = SPAN_FromToken(*param->idParamName);

        // copy over all parameter specific properties
        newParamSymbol->symType     = paramType;
        newParamSymbol->passingType = passingType;
        newParamSymbol->isOptional  = isOptional;
        newParamSymbol->bucketIndex = bucketIdx;

        // try to add it to the parameter list
        if (!BINDER_addParameterToProcedureSymbol(me, symModule, symProc, newParamSymbol))
            break;
    }

}

void BINDER_indexLabels(binder_t *me, module_symbol_t *symModule, procedure_symbol_t *thisProc, ls_ast_node_list_t statementList) {

    // go through all statements in this block
    for (int i = 0; i < statementList.length; ++i) {

        // see if we find any label statements
        if (statementList.nodes[i]->type == LS_LABEL_STATEMENT) {
            ls_label_statement_node_t *stmtLabel = (ls_label_statement_node_t*)statementList.nodes[i];

            // create a new label symbol based on the statement
            label_symbol_t *newLabel = malloc(sizeof(label_symbol_t));

            // set up the new label
            newLabel->base.type = LABEL_SYMBOL;
            strcpy(newLabel->base.name, stmtLabel->idLabel->strValue);
            newLabel->base.declarationSpan = SPAN_FromToken(*stmtLabel->idLabel);

            // set its id based on the current label list in out procedure
            newLabel->labelId = thisProc->lsLabels.length;

            // try to add it to the procedure
            if (!BINDER_addLabelToProcedureSymbol(me, symModule, thisProc, newLabel)) {
                return; // something went wrong
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------
bool BINDER_addModuleToProgramUnit(binder_t *me, program_unit_t *programUnit, module_symbol_t *newModuleSymbol) {

    // does a module with same name already exist in the program unit?
    int idx = BD_SYMBOL_LIST_Find(&programUnit->lsModules, newModuleSymbol->base.name);

    // add it to the list, even if theres duplicates
    // this is done so it can be cleaned up more easely in a central point after the error was propagated
    BD_SYMBOL_LIST_Add(&programUnit->lsModules, (symbol_t*)newModuleSymbol);

    // did we find another instance?
    if (idx != -1) {
        ERROR_SPLICE(SUB_BINDER, ERR_BD_NON_UNIQUE_SYMBOL, "A module of the name '%s' has already been registered", newModuleSymbol->base.name);
        me->hasError = true;
        return false;
    }

    return true;
}

bool BINDER_addSymbolToModuleSymbol(binder_t *me, module_symbol_t *symModule, symbol_t *newSymbol) {

    if (newSymbol == NULL) return false;

    // does this module already have a symbol by the same name?
    // --------------------------------------------------------
    int idxInTypes, idxInProcedures;

    // check types, functions and subroutines
    idxInTypes = BD_SYMBOL_LIST_Find(&symModule->lsTypes, newSymbol->name);
    idxInProcedures = BD_SYMBOL_LIST_Find(&symModule->lsProcedures, newSymbol->name);

    // add this symbol to the module, even if the name is already taken
    // this is so it can be cleaned up once the error has been propagated
    switch (newSymbol->type) {
        case TYPE_SYMBOL: BD_SYMBOL_LIST_Add(&symModule->lsTypes, newSymbol); break;
        case PROCEDURE_SYMBOL: BD_SYMBOL_LIST_Add(&symModule->lsProcedures, newSymbol); break;
        default:;
    }

    // was a type with this name found?
    if (idxInTypes != -1) {
        ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_NON_UNIQUE_SYMBOL, symModule->source, newSymbol->declarationSpan, "The name '%s' has already been used by a type member in this module also called '%s'", newSymbol->name, newSymbol->name);
        me->hasError = true;
        return false;
    }

    // was a procedure with this name found?
    if (idxInProcedures != -1) {
        ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_NON_UNIQUE_SYMBOL, symModule->source, newSymbol->declarationSpan, "The name '%s' has already been used by a function or subroutine member in this module also called '%s'", newSymbol->name, newSymbol->name);
        me->hasError = true;
        return false;
    }

    return true;
}

bool BINDER_addFieldToTypeSymbol(binder_t *me, module_symbol_t *symModule, type_symbol_t *symType, type_field_symbol_t *newField) {

    // does a field with this name already exist?
    int idx = BD_SYMBOL_LIST_Find(&symType->lsFields, newField->base.name);

    // add it to the list weather or not the name is already taken
    // -> for clean up purposes
    BD_SYMBOL_LIST_Add(&symType->lsFields, (symbol_t*)newField);

    // report the error if there is doubling
    if (idx != -1) {
        ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_NON_UNIQUE_SYMBOL, symModule->source, newField->base.declarationSpan, "The name '%s' has already been used by another field in this type also called '%s'", newField->base.name, newField->base.name);
        me->hasError = true;
        return false;
    }

    return true;
}

bool BINDER_addParameterToProcedureSymbol(binder_t *me, module_symbol_t *symModule, procedure_symbol_t *symProc, parameter_symbol_t *newParameter) {

    bool ok = true;

    // does this parameter have the same name as the function?
    // not allowed -> name is reserved for the return value
    // NOTE: this would already be handled by the check in the buckets list but i want a nicer error for this
    // edge case
    if (strcmp(symProc->base.name, newParameter->base.name) == 0) {
        ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_NON_UNIQUE_SYMBOL, symModule->source, newParameter->base.declarationSpan, "The name '%s' has already been used as the function or subroutine name and is therefore reserved", newParameter->base.name);
        me->hasError = true;
        ok = false;
    }

    // has this name already been taken by another variable?
    if (ok) {
        int idxBucket = BD_SYMBOL_LIST_Find(&symProc->lsBuckets, newParameter->base.name);
        if (idxBucket != -1) {
            ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_NON_UNIQUE_SYMBOL, symModule->source, newParameter->base.declarationSpan, "The name '%s' has already been used by a previous parameter or local variable", newParameter->base.name);
            me->hasError = true;
            ok = false;
        }
    }

    // add the symbol to the list, no matter if there has been an issue or not
    // this is done for clean up purposes
    BD_SYMBOL_LIST_Add(&symProc->lsParameters, (symbol_t*)newParameter);
    BD_SYMBOL_LIST_Add(&symProc->lsBuckets, (symbol_t*)newParameter);

    return ok;
}

bool BINDER_addLocalVariableToProcedureSymbol(binder_t *me, module_symbol_t *symModule, procedure_symbol_t *symProc, local_variable_symbol_t *newLocalVariable) {

    bool ok = true;

    // does this local have the same name as the function?
    // not allowed -> name is reserved for the return value
    // NOTE: this would already be handled by the check in the buckets list but i want a nicer error for this
    // edge case
    if (strcmp(symProc->base.name, newLocalVariable->base.name) == 0) {
        ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_NON_UNIQUE_SYMBOL, symModule->source, newLocalVariable->base.declarationSpan, "The name '%s' has already been used as the function or subroutine name and is therefore reserved", newLocalVariable->base.name);
        me->hasError = true;
        ok = false;
    }

    // has this name already been taken by another variable?
    if (ok) {
        int idxBucket = BD_SYMBOL_LIST_Find(&symProc->lsBuckets, newLocalVariable->base.name);
        if (idxBucket != -1) {
            ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_NON_UNIQUE_SYMBOL, symModule->source, newLocalVariable->base.declarationSpan, "The name '%s' has already been used by a previous parameter or local variable", newLocalVariable->base.name);
            me->hasError = true;
            ok = false;
        }
    }

    // add the symbol to the list, no matter if there has been an issue or not
    // this is done for clean up purposes
    BD_SYMBOL_LIST_Add(&symProc->lsBuckets, (symbol_t*)newLocalVariable);

    return ok;
}

bool BINDER_addLabelToProcedureSymbol(binder_t *me, module_symbol_t *symModule, procedure_symbol_t *symProc, label_symbol_t *newLabel) {

    // is there already a label with this name?
    int idx = BD_SYMBOL_LIST_Find(&symProc->lsLabels, newLabel->base.name);

    // add it to the list
    BD_SYMBOL_LIST_Add(&symProc->lsLabels, (symbol_t*)newLabel);

    // throw the relevant error
    if (idx != -1) {
        ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_NON_UNIQUE_SYMBOL, symModule->source, newLabel->base.declarationSpan, "The name '%s' has already been used by a previously declared label", newLabel->base.name);
        me->hasError = true;
        return false;
    }

    return true;
}

label_symbol_t *BINDER_generateLabel(binder_t *me, module_symbol_t *symModule, procedure_symbol_t *symProc, span_t declaration, char *name) {

    // create a new label symbol
    label_symbol_t *newLabel = malloc(sizeof(label_symbol_t));

    // set up the new label
    newLabel->base.type = LABEL_SYMBOL;
    newLabel->base.declarationSpan = declaration;

    // set its id based on the current label list in out procedure
    newLabel->labelId = symProc->lsLabels.length;

    // construct a name for this label
    snprintf(newLabel->base.name, MAX_IDENTIFIER_LENGTH, "#%s_%d", name, newLabel->labelId);

    // add it to the procedure label list
    BINDER_addLabelToProcedureSymbol(me, symModule, symProc, newLabel);

    return newLabel;
}