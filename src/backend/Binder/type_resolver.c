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
// takes an as clause and returns the type symbol for it
type_symbol_t *BINDER_ResolveAsClause(binder_t *me, module_symbol_t *symMod, ls_as_clause_node_t *clsAs) {

    type_symbol_t *baseType = BINDER_resolveTypeName(me, symMod, clsAs->idType);
    RETURN_NULL_ON_ERROR()

    // if this is not an array type -> we are so done
    if (clsAs->pcOpenParenthesis == NULL) {
        return baseType;
    }

    // if this is an array type:
    // find out how many dimensions this array has
    uint8_t dimensions = 1;
    if (clsAs->lsArrRanges.length > 1)
        dimensions = clsAs->lsArrRanges.length;

    // make sure this type is actually valid
    if (dimensions > MAX_ARRAY_DIMENSIONS) {
        ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_TOO_MANY_ARRAY_DIMENSIONS, symMod->source, SPAN_Between(SPAN_FromToken(*clsAs->pcOpenParenthesis), SPAN_FromToken(*clsAs->pcClosedParenthesis)), "Array types only allow for a maximum of %d dimensions", MAX_ARRAY_DIMENSIONS)
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

    // give it the same name as the base type
    strcpy(arrType->base.name, baseType->base.name);

    // give it the desired amount of dimensions
    arrType->numArrayDimensions = dimensions;

    // add it to the global table
    BD_SYMBOL_LIST_Add(&me->programUnit->lsArrayTypes, (symbol_t*)arrType);

    // return it back
    return arrType;
}

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