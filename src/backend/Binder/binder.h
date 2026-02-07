//
// Created by ds on 1/29/26.
//

#ifndef BINDER_H
#define BINDER_H
#include "AST/Loose/ls_ast.h"
#include "AST/Loose/Statements/assignment_statement.h"
#include "AST/Loose/Statements/call_statement.h"
#include "AST/Loose/Statements/dim_statement.h"
#include "AST/Loose/Statements/do_statement.h"
#include "AST/Loose/Statements/exit_statement.h"
#include "AST/Loose/Statements/expression_statement.h"
#include "AST/Loose/Statements/for_statement.h"
#include "AST/Loose/Statements/goto_statement.h"
#include "AST/Loose/Statements/if_statement.h"
#include "AST/Loose/Statements/label_statement.h"
#include "AST/Loose/Statements/redim_statement.h"
#include "AST/Loose/Statements/select_statement.h"
#include "AST/Loose/Statements/while_statement.h"
#include "AST/Loose/Expressions/binary_expression.h"
#include "AST/Loose/Expressions/unary_expression.h"
#include "AST/Loose/Expressions/literal_expression.h"
#include "AST/Loose/Expressions/parenthesized_expression.h"
#include "AST/Loose/Expressions/reference_expression.h"
#include "AST/Tight/program_unit.h"
#include "AST/Tight/tg_ast.h"
#include "AST/Tight/Symbols/module_symbol.h"
#include "AST/Tight/Symbols/procedure_symbol.h"
#include "AST/Tight/Symbols/type_symbol.h"
#include "Lexer/source.h"

#define MAX_ARRAY_DIMENSIONS 60
#define MAX_VARIADIC_ARGS 100

typedef struct BINDER {

    // the program unit we are building
    program_unit_t *programUnit;

    // have we hit an error while binding?
    bool hasError;

    // the current module we're in
    module_symbol_t *currentModule;

    // the current procedure we're in
    procedure_symbol_t *currentProcedure;

    // a stack of For loop break labels
    symbol_list_t stkForBreakLabels;

    // a stack of Do loop break labels
    symbol_list_t stkDoBreakLabels;

} binder_t;

binder_t *BINDER_Init();
void BINDER_Unload(binder_t *me);

// ---------------------------------------------------------------------------------------------------------------------
void BINDER_BindProcedureBodies(binder_t *me);

tg_ast_node_list_t BINDER_bindStatementList(binder_t *me, ls_ast_node_list_t statements);
tg_ast_node_t *BINDER_bindStatement(binder_t *me, ls_ast_node_t *statement, tg_ast_node_list_t *boundStatementList);
tg_ast_node_t *BINDER_bindAssignmentStatement(binder_t *me, ls_assignment_statement_node_t *statement);
tg_ast_node_t *BINDER_bindCallStatement(binder_t *me, ls_call_statement_node_t *statement);
tg_ast_node_t *BINDER_bindDimStatement(binder_t *me, ls_dim_statement_node_t *statement, tg_ast_node_list_t *boundStatementList);
tg_ast_node_t *BINDER_bindDoStatement(binder_t *me, ls_do_statement_node_t *statement);
tg_ast_node_t *BINDER_bindExitStatement(binder_t *me, ls_exit_statement_node_t *statement);
tg_ast_node_t *BINDER_bindExpressionStatement(binder_t *me, ls_expression_statement_node_t *statement);
tg_ast_node_t *BINDER_bindForStatement(binder_t *me, ls_for_statement_node_t *statement);
tg_ast_node_t *BINDER_bindGotoStatement(binder_t *me, ls_goto_statement_node_t *statement);
tg_ast_node_t *BINDER_bindIfStatement(binder_t *me, ls_if_statement_node_t *statement);
tg_ast_node_t *BINDER_bindLabelStatement(binder_t *me, ls_label_statement_node_t *statement);
tg_ast_node_t *BINDER_bindRedimStatement(binder_t *me, ls_redim_statement_node_t *statement);
tg_ast_node_t *BINDER_bindSelectStatement(binder_t *me, ls_select_statement_node_t *statement);
tg_ast_node_t *BINDER_bindWhileStatement(binder_t *me, ls_while_statement_node_t *statement);

tg_ast_node_list_t BINDER_bindCallArguments(binder_t *me, procedure_symbol_t *target, ls_ast_node_list_t arguments, span_t errorLocation);

tg_ast_node_t *BINDER_bindExpression(binder_t *me, ls_ast_node_t *expression, bool requireValue);

tg_ast_node_t *BINDER_bindBinaryExpression(binder_t *me, ls_binary_expression_node_t *expression);
tg_ast_node_t *BINDER_bindUnaryExpression(binder_t *me, ls_unary_expression_node_t *expression);
tg_ast_node_t *BINDER_bindParenthesizedExpression(binder_t *me, ls_parenthesized_expression_node_t *expression);
tg_ast_node_t *BINDER_bindReferenceExpression(binder_t *me, ls_reference_expression_node_t *expression);
tg_ast_node_t *BINDER_bindLiteralExpression(binder_t *me, ls_literal_expression_node_t *expression);


tg_ast_node_t *BINDER_bindDefaultExpression(binder_t *me, type_symbol_t *type);
tg_ast_node_t *BINDER_bindEnsureTypeExpression(binder_t *me, type_symbol_t *type, tg_ast_node_t *expression);

tg_ast_node_t *BINDER_createIntegerLiteralExpression(binder_t *me, int16_t value);

#endif //BINDER_H
