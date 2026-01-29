//
// Created by ds on 1/29/26.
//
#include <stddef.h>
#include "indexer.h"
#include "binder.h"

#include <stdlib.h>
#include <string.h>

#include "type_resolver.h"
#include "AST/Tight/Symbols/type_field_symbol.h"
#include "AST/Loose/Clauses/type_field_clause.h"
#include "AST/Loose/Members/type_member.h"
#include "Error/error.h"
#include "Parser/parser.h"


// ---------------------------------------------------------------------------------------------------------------------
// BINDER_CreateModuleIndex:
// go over all parsed members, configure this module, and create symbols for all types
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
        newModule->base.type = LS_MODULE_MEMBER;

        // set the name to the default
        strcpy(newModule->base.name, BD_DEFAULT_MODULE_NAME);

        // this is an automatically inserted module
        newModule->isAutoInserted = true;

        // initialize all lists
        newModule->lsFunctions = BD_SYMBOL_LIST_Init();
        newModule->lsSubroutines = BD_SYMBOL_LIST_Init();
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
            BD_SYMBOL_LIST_Add(&newModule->lsTypes, (symbol_t*)BINDER_indexTypeName(memberNode, i));
        }
    }

    BD_SYMBOL_LIST_Add(&me->programUnit->lsModules, (symbol_t*)newModule);
}

// ---------------------------------------------------------------------------------------------------------------------
// BINDER_CreateTypeIndex:
// go over all parsed members, build up the type fields
void BINDER_CreateTypeIndex(binder_t *me, module_symbol_t *symModule) {

    for (int i = 0; i < symModule->lsTypes.length; ++i) {

        BINDER_indexTypeFields(me, symModule, (type_symbol_t*)symModule->lsTypes.symbols[i]);
    }
}

module_symbol_t *BINDER_indexModule(ls_module_member_node_t *memModule) {
    module_symbol_t *newModule = malloc(sizeof(module_symbol_t));
    newModule->base.type = LS_MODULE_MEMBER;

    // copy over the module name
    strcpy(newModule->base.name, memModule->idModuleName->strValue);

    // this is a user defined module
    newModule->isAutoInserted = false;

    // initialize all lists
    newModule->lsFunctions = BD_SYMBOL_LIST_Init();
    newModule->lsSubroutines = BD_SYMBOL_LIST_Init();
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
        type_symbol_t *fieldType = BINDER_ResolveAsClause(me, symModule, clsField->clsType);
        if (me->hasError) return;

        // create a new field symbol
        type_field_symbol_t *symField = malloc(sizeof(type_field_symbol_t));
        symField->base.type = TYPE_FIELD_SYMBOL;

        // copy the field name over
        strcpy(symField->base.name, clsField->idFieldName->strValue);

        // assign the type
        symField->symType = fieldType;

        // add it to our user defined type symbol
        BD_SYMBOL_LIST_Add(&symType->lsFields, (symbol_t*)symField);
    }

}

function_symbol_t *BINDER_indexFunction(ls_function_member_node_t *memFunction) {

}

subroutine_symbol_t *BINDER_indexSubroutine(ls_sub_member_node_t *memSubroutine) {

}