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
#include "AST/Tight/Symbols/function_symbol.h"
#include "AST/Tight/Symbols/subroutine_symbol.h"
#include "AST/Tight/Symbols/type_symbol.h"

// program_unit_t *BINDER_CreateIndex(ls_ast_node_list_t members);
void BINDER_CreateModuleIndex(binder_t *me, source_t source, ls_ast_node_list_t lsMembers);
void BINDER_CreateTypeIndex(binder_t *me, module_symbol_t *symModule);
module_symbol_t *BINDER_indexModule(ls_module_member_node_t *memModule);
type_symbol_t *BINDER_indexTypeName(ls_type_member_node_t *memType, int idx);
void BINDER_indexTypeFields(binder_t *me, module_symbol_t *symModule, type_symbol_t *symType);
function_symbol_t *BINDER_indexFunction(ls_function_member_node_t *memFunction);
subroutine_symbol_t *BINDER_indexSubroutine(ls_sub_member_node_t *memSubroutine);

#endif //INDEXER_H
