//
// Created by ds on 2/10/26.
//

#ifndef INTEROP_H
#define INTEROP_H
#include "AST/Tight/program_unit.h"
#include "AST/Tight/Symbols/module_symbol.h"
#include "AST/Tight/Symbols/parameter_symbol.h"
#include "AST/Tight/Symbols/procedure_symbol.h"

type_symbol_t *INTEROP_LookupType(program_unit_t *prgUnit, char *name);
module_symbol_t *INTEROP_CreateModule(program_unit_t *prgUnit, char *moduleName);
procedure_symbol_t *INTEROP_CreateProcedure(program_unit_t *prgUnit, module_symbol_t *module, char *procName, type_symbol_t *returnType);
void INTEROP_AddParameter(procedure_symbol_t *procedure, char *parameterName, type_symbol_t *parameterType, passing_type_t passingType, bool isOptional);

#endif //INTEROP_H
