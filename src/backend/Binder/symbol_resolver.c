//
// Created by ds on 1/29/26.
//
#include "symbol_resolver.h"

#include <stdlib.h>
#include <string.h>

#include "AST/Tight/Symbols/module_symbol.h"
#include "Error/error.h"
#include "Parser/parser.h"

// ---------------------------------------------------------------------------------------------------------------------
// BINDER_ResolveAsClause:
// takes an as clause and returns the type symbol for it, creates array types if needed
type_symbol_t *BINDER_ResolveAsClause(binder_t *me, module_symbol_t *symMod, ls_as_clause_node_t *clsAs, array_resolve_behavior_t arrayBehavior) {

    // if the As clause is null -> return a variant type
    if (clsAs == NULL) return (type_symbol_t*)me->programUnit->lsBuiltinTypes.symbols[IDX_BUILTIN_VARIANT];

    // otherwise: resolve the base type
    type_symbol_t *baseType = BINDER_resolveTypeName(me, symMod, clsAs->idType);
    RETURN_NULL_ON_ERROR()

    // if this is not an array type -> we are so done
    if (clsAs->pcOpenParenthesis == NULL) {
        return baseType;
    }

    // are we allowed to use array types here?
    if (arrayBehavior == BD_RS_NO_ARRAYS) {
        ERROR_AT(SUB_BINDER, ERR_BD_UNEXPECTED_ARRAY_TYPE, symMod->source, SPAN_Between(SPAN_FromToken(*clsAs->pcOpenParenthesis), SPAN_FromToken(*clsAs->pcClosedParenthesis)), "Array types are not allowed to be used here")
        me->hasError = true;
        return NULL;
    }

    // if this is an array type:
    // find out how many dimensions this array has
    uint8_t dimensions = ARRAY_DIM_GENERIC;
    if (clsAs->lsArrRanges.length > 0)
        dimensions = clsAs->lsArrRanges.length;

    // make sure this type is actually valid
    if (dimensions > MAX_ARRAY_DIMENSIONS) {
        ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_TOO_MANY_ARRAY_DIMENSIONS, symMod->source, SPAN_Between(SPAN_FromToken(*clsAs->pcOpenParenthesis), SPAN_FromToken(*clsAs->pcClosedParenthesis)), "Array types only allow for a maximum of %d dimensions", MAX_ARRAY_DIMENSIONS)
        me->hasError = true;
        return NULL;
    }

    // does the context we're resolving types in even allow array ranges?
    if (dimensions > ARRAY_DIM_GENERIC && arrayBehavior == BD_RS_ALLOW_ONLY_GENERIC_SIZE_ARRAYS) {
        ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_ARRAY_TYPE_MUST_BE_GENERIC, symMod->source, SPAN_Between(SPAN_FromToken(*clsAs->pcOpenParenthesis), SPAN_FromToken(*clsAs->pcClosedParenthesis)), "This context only allows for generic arrays", MAX_ARRAY_DIMENSIONS)
        me->hasError = true;
        return NULL;
    }

    // look if a type symbol for this array already exits
    for (int i = 0; i < me->programUnit->lsArrayTypes.length; ++i) {

        // look for an array symbol with the same name
        if (strcmp(me->programUnit->lsArrayTypes.symbols[i]->name, baseType->base.name) == 0) {

            type_symbol_t *arrType = (type_symbol_t*)me->programUnit->lsArrayTypes.symbols[i];

            // and the same number of dimensions
            if (arrType->numArrayDimensions == dimensions) {

                // found it!
                return arrType;
            }
        }
    }

    // if it doesnt exist yet -> create a new one and add it to the list
    type_symbol_t *arrType = malloc(sizeof(type_symbol_t));
    arrType->base.type = TYPE_SYMBOL;
    arrType->typeOfType = TYPE_ARRAY;

    // create a new name for this type
    snprintf(arrType->base.name, MAX_IDENTIFIER_LENGTH, "%s_array_dim_%d", baseType->base.name, dimensions);

    // give it the desired amount of dimensions
    arrType->numArrayDimensions = dimensions;

    // link the original type
    arrType->symSubType = baseType;

    // add it to the global table
    BD_SYMBOL_LIST_Add(&me->programUnit->lsArrayTypes, (symbol_t*)arrType);

    // return it back
    return arrType;
}

// ---------------------------------------------------------------------------------------------------------------------
// BINDER_resolveTypeName:
// resolves a type by its name, passed as a token for error reporting
type_symbol_t *BINDER_resolveTypeName(binder_t *me, module_symbol_t *symMod, token_t *idTypeName) {

    // if the name is null -> return a variant type
    if (idTypeName == NULL) return (type_symbol_t*)me->programUnit->lsBuiltinTypes.symbols[IDX_BUILTIN_VARIANT];

    // -----------------------------------------------------------------------------------------------------------------
    // check if this is a builtin symbol
    for (int i = 0; i < me->programUnit->lsBuiltinTypes.length; i++) {
        if (strcmp(me->programUnit->lsBuiltinTypes.symbols[i]->name, idTypeName->strValue) == 0) {

            // found it!
            return (type_symbol_t*)me->programUnit->lsBuiltinTypes.symbols[i];
        }
    }

    // -----------------------------------------------------------------------------------------------------------------
    // check if this is a user defined type

    // go through each module
    for (int m = 0; m < me->programUnit->lsModules.length; ++m) {

        module_symbol_t *symModule = (module_symbol_t*)me->programUnit->lsModules.symbols[m];

        // go through all user defined types
        for (int t = 0; t < symModule->lsTypes.length; ++t) {

            // is this it?
            if (strcmp(symModule->lsTypes.symbols[t]->name, idTypeName->strValue) == 0) {

                // yup
                return (type_symbol_t*)symModule->lsTypes.symbols[t];
            }
        }
    }

    // Oh no!
    ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_UNRECOGNIZED_TYPE_NAME, symMod->source, SPAN_FromToken(*idTypeName), "The type name '%s' could not be resolved to a type", idTypeName->strValue);
    me->hasError = true;

    return NULL;
}

type_symbol_t *BINDER_resolveTypeNameFromBuffer(binder_t *me, char *buffer) {
    // -----------------------------------------------------------------------------------------------------------------
    // check if this is a builtin symbol
    for (int i = 0; i < me->programUnit->lsBuiltinTypes.length; i++) {
        if (strcmp(me->programUnit->lsBuiltinTypes.symbols[i]->name, buffer) == 0) {

            // found it!
            return (type_symbol_t*)me->programUnit->lsBuiltinTypes.symbols[i];
        }
    }

    // anything else is not available at compile time
    ERROR_SPLICE(SUB_BINDER, ERR_INTERNAL, "The type name '%s' could not be resolved to a builtin type", buffer);
    me->hasError = true;

    return NULL;
}

procedure_symbol_t *BINDER_ResolveProcedure(binder_t *me, ls_reference_expression_node_t *exprReference) {

    // is this just a procedure name on its own?
    if (exprReference->exprBase == NULL) {
        return BINDER_resolveProcedureName(me, me->currentModule, exprReference->idName);
    }

    // -----------------------------------------------------------------------------------------------------------------
    // otherwise this should be a "Module.Procedure"

    // make sure the inner layer is also a name expression
    if (exprReference->exprBase->type != LS_REFERENCE_EXPRESSION) {
        ERROR_AT(SUB_BINDER, ERR_BD_INVALID_REFERENCE_LINK, me->currentModule->source, SPAN_FromToken(*exprReference->idName), "The reference path for a procedure call must only contain named links");
        me->hasError = true;
        return NULL;
    }

    // also make sure we dont have too many layers, only two are allowed
    ls_reference_expression_node_t *exprInner = (ls_reference_expression_node_t*)exprReference->exprBase;

    if (exprInner->exprBase != NULL) {
        ERROR_AT(SUB_BINDER, ERR_BD_TOO_MANY_REFERENCE_LINKS, me->currentModule->source, SPAN_FromToken(*exprInner->idName), "The reference path for a procedure call must only be of a maximum length of 2");
        me->hasError = true;
        return NULL;
    }

    // resolve the module
    module_symbol_t *module = BINDER_resolveModuleName(me, exprInner->idName);
    if (module == NULL) return NULL; // we did not find the module

    // resolve the procedure inside the module
    procedure_symbol_t *proc = BINDER_resolveProcedureName(me, module, exprReference->idName);
    return proc;
}


procedure_symbol_t *BINDER_resolveProcedureName(binder_t *me, module_symbol_t *symModule, token_t *procedureName) {

    // try to find the procedure by name inside the given module
    int idx = BD_SYMBOL_LIST_Find(&symModule->lsProcedures, procedureName->strValue);

    // did we get nothing?
    if (idx == -1) {
        ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_UNKNOWN_PROCEDURE_NAME, me->currentModule->source, SPAN_FromToken(*procedureName), "Could not find a procedure named '%s' in module '%s'", procedureName->strValue, symModule->base.name);
        me->hasError = true;
        return NULL;
    }

    // otherwise: return the procedure symbol
    return (procedure_symbol_t*)symModule->lsProcedures.symbols[idx];
}

module_symbol_t *BINDER_resolveModuleName(binder_t *me, token_t *moduleName) {

    // try to find the module by name inside the program unit
    int idx = BD_SYMBOL_LIST_Find(&me->programUnit->lsModules, moduleName->strValue);

    // did we get nothing?
    if (idx == -1) {
        ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_UNKNOWN_MODULE_NAME, me->currentModule->source, SPAN_FromToken(*moduleName), "Could not find a module named '%s'", moduleName->strValue);
        me->hasError = true;
        return NULL;
    }

    // otherwise: return the module symbol
    return (module_symbol_t*)me->programUnit->lsModules.symbols[idx];
}

label_symbol_t *BINDER_ResolveLabelName(binder_t *me, module_symbol_t *symMod, procedure_symbol_t *symProc, token_t *idLabelName) {

    // try to find the label in this procedure
    int idx = BD_SYMBOL_LIST_Find(&symProc->lsLabels, idLabelName->strValue);

    // did we get nothing?
    if (idx == -1) {
        ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_UNKNOWN_LABEL_NAME, me->currentModule->source, SPAN_FromToken(*idLabelName), "Could not find a label named '%s' withing procedure '%s'", idLabelName->strValue, symProc->base.name);
        me->hasError = true;
        return NULL;
    }

    // otherwise: return the label
    return (label_symbol_t*)symProc->lsLabels.symbols[idx];
}

bool BINDER_areTypesEqual(type_symbol_t *a, type_symbol_t *b) {
    return a == b; // same types should have the same pointer address (i think)
}

uint32_t BINDER_getSizeForLiteralType(token_t *ltToken) {

    // is this a boolean literal?
    if (ltToken->type == TK_LT_BOOLEAN) {
        return 1; // always 1 byte
    }

    // is this a string literal
    if (ltToken->type == TK_LT_STRING) {
        return strlen(ltToken->strValue) + 1;
    }

    // is this a numeric literal?
    if (ltToken->type == TK_LT_NUMBER) {

        // what kind of number is it exaclty?
        switch (ltToken->numberValueType) {
            case NUMBER_VALUE_BYTE      : return sizeof(uint8_t);
            case NUMBER_VALUE_INT       : return sizeof(int16_t);
            case NUMBER_VALUE_LONG      : return sizeof(int32_t);
            case NUMBER_VALUE_LONG_LONG : return sizeof(int64_t);
            case NUMBER_VALUE_SINGLE    : return sizeof(float);
            case NUMBER_VALUE_DOUBLE    : return sizeof(double);
            case NUMBER_VALUE_CURRENCY  : return sizeof(int64_t);
            default:
                ERROR_SPLICE(SUB_BINDER, ERR_INTERNAL, "Unknown number literal type '%d'", ltToken->numberValueType);
                return 1;
        }
    }

    ERROR_SPLICE(SUB_BINDER, ERR_INTERNAL, "Unknown literal type '%s'", TOKEN_TYPE_STRING[ltToken->type]);
    return 1;
}

type_symbol_t *BINDER_getTypeForLiteral(binder_t *me, token_t *ltToken) {

    // is this a boolean literal?
    if (ltToken->type == TK_LT_BOOLEAN) {
        return BINDER_resolveTypeNameFromBuffer(me, "boolean");
    }

    // is this a string literal
    if (ltToken->type == TK_LT_STRING) {
        return BINDER_resolveTypeNameFromBuffer(me, "string");
    }

    // is this a numeric literal?
    if (ltToken->type == TK_LT_NUMBER) {

        // what kind of number is it exaclty?
        switch (ltToken->numberValueType) {
            case NUMBER_VALUE_BYTE      : return BINDER_resolveTypeNameFromBuffer(me, "byte");
            case NUMBER_VALUE_INT       : return BINDER_resolveTypeNameFromBuffer(me, "integer");
            case NUMBER_VALUE_LONG      : return BINDER_resolveTypeNameFromBuffer(me, "long");
            case NUMBER_VALUE_LONG_LONG : return BINDER_resolveTypeNameFromBuffer(me, "longlong");
            case NUMBER_VALUE_SINGLE    : return BINDER_resolveTypeNameFromBuffer(me, "single");
            case NUMBER_VALUE_DOUBLE    : return BINDER_resolveTypeNameFromBuffer(me, "double");
            case NUMBER_VALUE_CURRENCY  : return BINDER_resolveTypeNameFromBuffer(me, "currency");
            default:
                ERROR_SPLICE(SUB_BINDER, ERR_INTERNAL, "Unknown number literal type '%d'", ltToken->numberValueType);
            return BINDER_resolveTypeNameFromBuffer(me, "variant");
        }
    }

    ERROR_SPLICE(SUB_BINDER, ERR_INTERNAL, "Unknown literal type '%s'", TOKEN_TYPE_STRING[ltToken->type]);
    return BINDER_resolveTypeNameFromBuffer(me, "variant");
}