//
// Created by ds on 1/29/26.
//
#include "type_resolver.h"

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
    if (clsAs->lsArrRanges.length > 1)
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
        if (strcmp(me->programUnit->lsArrayTypes.symbols[i]->name, clsAs->idType->strValue) == 0) {

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