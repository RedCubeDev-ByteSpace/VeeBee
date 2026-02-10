//
// Created by ds on 1/29/26.
//

#ifndef SYMBOL_RESOLVER_H
#define SYMBOL_RESOLVER_H
#include "binder.h"
#include "AST/Tight/Symbols/type_symbol.h"
#include "AST/Loose/Clauses/as_clause.h"
#include "AST/Loose/Expressions/reference_expression.h"
#include "AST/Tight/Symbols/module_symbol.h"

typedef enum ARRAY_RESOLVE_BEHAVIOR {

    BD_RS_ALLOW_ALL_ARRAYS,
    BD_RS_ALLOW_ONLY_GENERIC_SIZE_ARRAYS,
    BD_RS_NO_ARRAYS,

} array_resolve_behavior_t;

type_symbol_t *BINDER_ResolveAsClause(binder_t *me, module_symbol_t *symMod, ls_as_clause_node_t *clsAs, array_resolve_behavior_t arrayBehavior);
type_symbol_t *BINDER_resolveTypeName(binder_t *me, module_symbol_t *symMod, token_t *idTypeName);
type_symbol_t *BINDER_resolveTypeNameFromBuffer(binder_t *me, char *buffer);
type_symbol_t *BINDER_resolveArrayType(binder_t *me, type_symbol_t *baseType, uint8_t dimensions);

procedure_symbol_t *BINDER_ResolveProcedure(binder_t *me, ls_reference_expression_node_t *exprReference);
procedure_symbol_t *BINDER_resolveProcedureName(binder_t *me, module_symbol_t *symModule, token_t *procedureName);
module_symbol_t *BINDER_resolveModuleName(binder_t *me, token_t *moduleName);

label_symbol_t *BINDER_ResolveLabelName(binder_t *me, module_symbol_t *symMod, procedure_symbol_t *symProc, token_t *idLabelName);

uint32_t BINDER_getSizeForLiteralType(token_t *ltToken);
type_symbol_t *BINDER_getTypeForLiteral(binder_t *me, token_t *ltToken);

bool BINDER_areTypesEqual(type_symbol_t *a, type_symbol_t *b);

#endif //SYMBOL_RESOLVER_H
