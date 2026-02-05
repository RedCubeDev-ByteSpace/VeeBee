//
// Created by ds on 1/29/26.
//

#ifndef SYMBOL_RESOLVER_H
#define SYMBOL_RESOLVER_H
#include "binder.h"
#include "AST/Tight/Symbols/type_symbol.h"
#include "AST/Loose/Clauses/as_clause.h"
#include "AST/Tight/Symbols/module_symbol.h"

typedef enum ARRAY_RESOLVE_BEHAVIOR {

    BD_RS_ALLOW_ALL_ARRAYS,
    BD_RS_ALLOW_ONLY_GENERIC_SIZE_ARRAYS,
    BD_RS_NO_ARRAYS,

} array_resolve_behavior_t;

type_symbol_t *BINDER_ResolveAsClause(binder_t *me, module_symbol_t *symMod, ls_as_clause_node_t *clsAs, array_resolve_behavior_t arrayBehavior);
type_symbol_t *BINDER_resolveTypeName(binder_t *me, module_symbol_t *symMod, token_t *idTypeName);

#endif //SYMBOL_RESOLVER_H
