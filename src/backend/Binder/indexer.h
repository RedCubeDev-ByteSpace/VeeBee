//
// Created by ds on 1/29/26.
//

#ifndef INDEXER_H
#define INDEXER_H
#include "binder.h"
#include "AST/Loose/Members/module_member.h"
#include "AST/Loose/Members/type_member.h"
#include "AST/Loose/Members/function_member.h"
#include "AST/Loose/Members/sub_member.h"
#include "AST/Loose/ls_ast.h"
#include "AST/Tight/program_unit.h"
#include "AST/Tight/Symbols/module_symbol.h"
#include "AST/Tight/Symbols/procedure_symbol.h"
#include "AST/Tight/Symbols/type_field_symbol.h"
#include "AST/Tight/Symbols/type_symbol.h"
#include "AST/Tight/Symbols/parameter_symbol.h"
#include "AST/Tight/Symbols/label_symbol.h"
#include "AST/Tight/Symbols/local_variable_symbol.h"

void BINDER_CreateModuleIndex(binder_t *me, source_t source, ls_ast_node_list_t lsMembers);
void BINDER_CreateTypeIndex(binder_t *me, module_symbol_t *symModule);
void BINDER_CreateProcedureIndex(binder_t *me, module_symbol_t *symModule);

bool BINDER_addModuleToProgramUnit(binder_t *me, program_unit_t *programUnit, module_symbol_t *newModuleSymbol);
bool BINDER_addSymbolToModuleSymbol(binder_t *me, module_symbol_t *symModule, symbol_t *newSymbol);
bool BINDER_addFieldToTypeSymbol(binder_t *me, module_symbol_t *symModule, type_symbol_t *symType, type_field_symbol_t *newField);
bool BINDER_addParameterToProcedureSymbol(binder_t *me, module_symbol_t *symModule, procedure_symbol_t *symProc, parameter_symbol_t *newParameter);
bool BINDER_addLocalVariableToProcedureSymbol(binder_t *me, module_symbol_t *symModule, procedure_symbol_t *symProc, local_variable_symbol_t *newLocalVariable);
bool BINDER_addLabelToProcedureSymbol(binder_t *me, module_symbol_t *symModule, procedure_symbol_t *symProc, label_symbol_t *newLabel);

label_symbol_t *BINDER_generateLabel(binder_t *me, module_symbol_t *symModule, procedure_symbol_t *symProc, span_t declaration, char *name);

module_symbol_t *BINDER_indexModule(ls_module_member_node_t *memModule);
type_symbol_t *BINDER_indexTypeName(ls_type_member_node_t *memType, int idx);
void BINDER_indexTypeFields(binder_t *me, module_symbol_t *symModule, type_symbol_t *symType);
procedure_symbol_t *BINDER_indexFunction(binder_t *me, module_symbol_t *symModule, ls_function_member_node_t *memFunction, int idx);
procedure_symbol_t *BINDER_indexSubroutine(binder_t *me, module_symbol_t *symModule, ls_sub_member_node_t *memSubroutine, int idx);
bool BINDER_validateProcedureParameters(binder_t *me, module_symbol_t *symModule, ls_ast_node_list_t lsParams);
void BINDER_indexProcedureParameters(binder_t *me, module_symbol_t *symModule, procedure_symbol_t *symProc, ls_ast_node_list_t lsParams, int startIdx);

void BINDER_indexLabels(binder_t *me, module_symbol_t *symModule, procedure_symbol_t *thisProc, ls_ast_node_list_t statementList);

#endif //INDEXER_H
