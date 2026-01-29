//
// Created by ds on 1/29/26.
//

#ifndef TYPE_RESOLVER_H
#define TYPE_RESOLVER_H
#include "binder.h"
#include "AST/Tight/Symbols/type_symbol.h"
#include "AST/Loose/Clauses/as_clause.h"
#include "AST/Tight/Symbols/module_symbol.h"

type_symbol_t *BINDER_ResolveAsClause(binder_t *me, module_symbol_t *symMod, ls_as_clause_node_t *clsAs);
type_symbol_t *BINDER_resolveTypeName(binder_t *me, module_symbol_t *symMod, token_t *idTypeName);

#endif //TYPE_RESOLVER_H
