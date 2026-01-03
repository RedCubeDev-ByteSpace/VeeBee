//
// Created by ds on 12/30/25.
//

#ifndef MODULE_MEMBER_H
#define MODULE_MEMBER_H

#include "../ls_ast.h"
#include "../../../Lexer/token.h"

// ---------------------------------------------------------------------------------------------------------------------
// 'Module' member
// ---------------
// Example:
// ] Module MyModule

typedef struct LS_MODULE_MEMBER_NODE {
    ls_ast_node_t base;

    token_t *kwModule;
    token_t *idModuleName;
} ls_module_member_node_t;

#endif //MODULE_MEMBER_H
