//
// Created by ds on 12/29/25.
//
#include <stdlib.h>
#include "ls_ast.h"
#include "../../Error/error.h"
#include "Clauses/conditional_clause.h"
#include "Clauses/dim_field_clause.h"

#include "Members/function_member.h"
#include "Members/sub_member.h"
#include "Members/type_member.h"
#include "Clauses/parameter_clause.h"
#include "Clauses/type_field_clause.h"
#include "Expressions/reference_expression.h"
#include "Statements/assignment_statement.h"
#include "Statements/dim_statement.h"
#include "Statements/do_statement.h"
#include "Statements/expression_statement.h"
#include "Statements/for_statement.h"
#include "Statements/if_statement.h"
#include "Statements/select_statement.h"
#include "Statements/redim_statement.h"
#include "Statements/while_statement.h"

// ---------------------------------------------------------------------------------------------------------------------
// Universal recursive node destructor
// ---------------------------------------------------------------------------------------------------------------------
void PS_LS_Node_Unload(ls_ast_node_t *me) {
    if (me == NULL) return; // nothing to do

    switch (me->type) {
        case LS_FUNCTION_MEMBER:
            PS_LS_AST_NODE_LIST_Unload(((ls_function_member_node_t*)me)->lsParameters);
            PS_LS_AST_NODE_LIST_Unload(((ls_function_member_node_t*)me)->lsFunctionBody);
            PS_LS_Node_Unload((ls_ast_node_t*)((ls_function_member_node_t*)me)->clsReturnType);
        break;

        case LS_SUBROUTINE_MEMBER:
            PS_LS_AST_NODE_LIST_Unload(((ls_sub_member_node_t*)me)->lsParameters);
            PS_LS_AST_NODE_LIST_Unload(((ls_sub_member_node_t*)me)->lsSubBody);
        break;

        case LS_TYPE_MEMBER:
            PS_LS_AST_NODE_LIST_Unload(((ls_type_member_node_t*)me)->lsFields);
        break;

        case LS_PARAMETER_CLAUSE:
            PS_LS_Node_Unload((ls_ast_node_t*)((ls_parameter_clause_node_t*)me)->clsType);
            PS_LS_Node_Unload(((ls_parameter_clause_node_t*)me)->expDefaultValue);
        break;

        case LS_TYPE_FIELD_CLAUSE:
            PS_LS_Node_Unload((ls_ast_node_t*)((ls_type_field_clause_node_t*)me)->clsType);
        break;

        case LS_AS_CLAUSE:
            PS_LS_AST_NODE_LIST_Unload(((ls_as_clause_node_t*)me)->lsArrRanges);
        break;

        case LS_DIM_STATEMENT:
            PS_LS_AST_NODE_LIST_Unload(((ls_dim_statement_node_t*)me)->lsDimFields);
        break;

        case LS_REDIM_STATEMENT:
            PS_LS_AST_NODE_LIST_Unload(((ls_redim_statement_node_t*)me)->lsDimFields);
        break;

        case LS_DIM_FIELD_CLAUSE:
            PS_LS_Node_Unload((ls_ast_node_t*)((ls_dim_field_clause_node_t*)me)->clsType);
        break;

        case LS_REFERENCE_EXPRESSION:
            PS_LS_Node_Unload(((ls_reference_expression_node_t*)me)->exprBase);
            PS_LS_AST_NODE_LIST_Unload(((ls_reference_expression_node_t*)me)->lsArguments);
        break;

        case LS_EXPRESSION_STATEMENT:
            PS_LS_Node_Unload(((ls_expression_statement_node_t*)me)->exprExpression);
        break;

        case LS_ASSIGNMENT_STATEMENT:
            PS_LS_Node_Unload(((ls_assignment_statement_node_t*)me)->exprTarget);
            PS_LS_Node_Unload(((ls_assignment_statement_node_t*)me)->exprValue);
        break;

        case LS_IF_STATEMENT:
            PS_LS_AST_NODE_LIST_Unload(((ls_if_statement_node_t*)me)->lsConditionals);
            PS_LS_Node_Unload((ls_ast_node_t*)((ls_if_statement_node_t*)me)->clsElse);
        break;

        case LS_CONDITIONAL_CLAUSE:
            PS_LS_Node_Unload(((ls_conditional_clause_node_t*)me)->exprCondition);
            PS_LS_AST_NODE_LIST_Unload(((ls_conditional_clause_node_t*)me)->lsStatements);
        break;

        case LS_ELSE_CLAUSE:
            PS_LS_AST_NODE_LIST_Unload(((ls_else_clause_node_t*)me)->lsStatements);
        break;

        case LS_SELECT_STATEMENT:
            PS_LS_Node_Unload(((ls_select_statement_node_t*)me)->exprTest);
            PS_LS_AST_NODE_LIST_Unload(((ls_select_statement_node_t*)me)->lsConditionals);
            PS_LS_Node_Unload((ls_ast_node_t*)((ls_select_statement_node_t*)me)->clsElse);
        break;

        case LS_FOR_STATEMENT:
            PS_LS_Node_Unload(((ls_for_statement_node_t*)me)->exprStart);
            PS_LS_Node_Unload(((ls_for_statement_node_t*)me)->exprEnd);
            PS_LS_Node_Unload(((ls_for_statement_node_t*)me)->exprStep);
            PS_LS_AST_NODE_LIST_Unload(((ls_for_statement_node_t*)me)->lsBody);
        break;

        case LS_WHILE_STATEMENT:
            PS_LS_Node_Unload(((ls_while_statement_node_t*)me)->exprCondition);
            PS_LS_AST_NODE_LIST_Unload(((ls_while_statement_node_t*)me)->lsBody);
        break;

        case LS_DO_STATEMENT:
            PS_LS_Node_Unload(((ls_do_statement_node_t*)me)->exprCondition);
            PS_LS_AST_NODE_LIST_Unload(((ls_do_statement_node_t*)me)->lsBody);
        break;

        default:;
    }

    // finally unload the buffer
    free(me);
}


// ---------------------------------------------------------------------------------------------------------------------
// Node List functions, basically a direct copy of the token list logic
// ---------------------------------------------------------------------------------------------------------------------

ls_ast_node_list_t PS_LS_AST_NODE_LIST_Init() {
    return (ls_ast_node_list_t) {
        NULL, 0, 0
    };
}

void PS_LS_AST_NODE_LIST_Unload(const ls_ast_node_list_t me) {
    for (int i = 0; i < me.length; ++i) {
        PS_LS_Node_Unload(me.nodes[i]);
    }

    free(me.nodes);
}

void PS_LS_AST_NODE_LIST_Add(ls_ast_node_list_t *me, ls_ast_node_t *newNode) {
    // grow the list buffer if needed
    if (!PS_LS_AST_NODE_LIST_grow(me)) {

        // aw man
        ERROR(SUB_PARSER, ERR_INTERNAL, "node list cannot grow any larger, out of memory");
        return;
    }

    // when theres sufficient space in the buffer -> add this entry
    me->nodes[me->length] = newNode;
    me->length++;
}

bool PS_LS_AST_NODE_LIST_grow(ls_ast_node_list_t *me) {
    if (me->length < me->capacity) return true; // nothing to do

    // otherwise: expand the capacity and reallocate the buffer
    me->capacity += PS_NODE_LIST_GROWTH;
    ls_ast_node_t **newBuffer = realloc(me->nodes, sizeof(ls_ast_node_t*) * me->capacity);

    // did we manage to allocate a new buffer?
    if (newBuffer == NULL) {
        // if not -> roll back the capacity and return false
        me->capacity -= PS_NODE_LIST_GROWTH;
        return false;
    }

    // otherwise, copy over the new buffer ptr into our list
    me->nodes = newBuffer;
    return true;
}