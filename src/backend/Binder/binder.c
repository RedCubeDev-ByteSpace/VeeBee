//
// Created by ds on 1/29/26.
//
#include "binder.h"

#include <stdlib.h>
#include <string.h>

#include "indexer.h"
#include "symbol_resolver.h"
#include "AST/Loose/Clauses/arr_range_clause.h"
#include "AST/Loose/Clauses/conditional_clause.h"
#include "AST/Loose/Clauses/dim_field_clause.h"
#include "AST/Loose/Members/function_member.h"
#include "AST/Tight/Symbols/module_symbol.h"
#include "AST/Tight/Symbols/procedure_symbol.h"
#include "AST/Tight/Statements/assignment_statement.h"
#include "AST/Tight/Statements/do_statement.h"
#include "AST/Tight/Statements/for_statement.h"
#include "AST/Tight/Statements/goto_statement.h"
#include "AST/Tight/Statements/if_statement.h"
#include "AST/Tight/Statements/initialize_statement.h"
#include "AST/Tight/Statements/label_statement.h"
#include "AST/Tight/Statements/proc_call_statement.h"
#include "AST/Tight/Statements/return_statement.h"
#include "AST/Tight/Statements/select_statement.h"
#include "AST/Tight/Statements/while_statement.h"
#include "AST/Tight/Expressions/binary_expression.h"
#include "AST/Tight/Expressions/default_expression.h"
#include "AST/Tight/Expressions/ensure_type_expression.h"
#include "AST/Tight/Expressions/literal_expression.h"
#include "AST/Tight/Expressions/reference_expression.h"
#include "AST/Tight/Expressions/unary_expression.h"
#include "Error/error.h"

binder_t *BINDER_Init() {
    binder_t *newBinder = malloc(sizeof(binder_t));

    // initialize the program unit
    newBinder->programUnit = BD_PROGRAM_UNIT_Init();

    // reset the error flag
    newBinder->hasError = false;

    // initialize the label stacks
    newBinder->stkDoBreakLabels = BD_SYMBOL_LIST_Init();
    newBinder->stkForBreakLabels = BD_SYMBOL_LIST_Init();

    return newBinder;
}

void BINDER_Unload(binder_t *me) {
    free(me);
}

void BINDER_BindProcedureBodies(binder_t *me) {

    // go through all modules
    for (int i = 0; i < me->programUnit->lsModules.length; ++i) {
        module_symbol_t *module = (module_symbol_t*)me->programUnit->lsModules.symbols[i];

        // tell the binder the module we'll be working in
        me->currentModule = module;

        // go through all procedures
        for (int p = 0; p < module->lsProcedures.length; ++p) {
            procedure_symbol_t *proc = (procedure_symbol_t*)module->lsProcedures.symbols[p];

            // tell the binder the proc we'll be working in
            me->currentProcedure = proc;

            // find the original parsed body
            ls_ast_node_t *member = module->lsMembers.nodes[proc->idxMember];
            ls_ast_node_list_t procedureBody;

            if (member->type == LS_FUNCTION_MEMBER)
                procedureBody = ((ls_function_member_node_t*)member)->lsFunctionBody;
            else if (member->type == LS_SUBROUTINE_MEMBER)
                procedureBody = ((ls_sub_member_node_t*)member)->lsSubBody;
            else
                continue;

            // bind the body as a block of statements
            tg_ast_node_list_t body = BINDER_bindStatementList(me, procedureBody);

            // store the body in the global procedure body buffer
            me->programUnit->bufProcedureBodies[proc->procedureId] = body;
        }
    }

}

tg_ast_node_list_t BINDER_bindStatementList(binder_t *me, ls_ast_node_list_t statements) {

    // first, before we bind any statement:
    // scan through this block for any labels that may have been defined
    BINDER_indexLabels(me, me->currentModule, me->currentProcedure, statements);

    // -----------------------------------------------------------------------------------------------------------------
    // then: iterate through this block and bind all statements

    // create a list for our bound statements
    tg_ast_node_list_t boundStatements = BD_TG_AST_NODE_LIST_Init();

    for (int i = 0; i < statements.length; ++i) {
        tg_ast_node_t *boundStatement = BINDER_bindStatement(me, statements.nodes[i], &boundStatements);

        // has there been an error? -> try the next statement
        if (boundStatement == NULL) continue;

        // if not -> add it to the list
        BD_TG_AST_NODE_LIST_Add(&boundStatements, boundStatement);
    }

    // return our newly bound statement list
    return boundStatements;
}

tg_ast_node_t *BINDER_bindStatement(binder_t *me, ls_ast_node_t *statement, tg_ast_node_list_t *boundStatementList) {

    // bind this node based on what type it is
    switch (statement->type) {
        case LS_DIM_STATEMENT        : return BINDER_bindDimStatement(me, (ls_dim_statement_node_t*)statement, boundStatementList);
        case LS_REDIM_STATEMENT      : return BINDER_bindRedimStatement(me, (ls_redim_statement_node_t*)statement);
        case LS_ASSIGNMENT_STATEMENT : return BINDER_bindAssignmentStatement(me, (ls_assignment_statement_node_t*)statement);
        case LS_IF_STATEMENT         : return BINDER_bindIfStatement(me, (ls_if_statement_node_t*)statement);
        case LS_SELECT_STATEMENT     : return BINDER_bindSelectStatement(me, (ls_select_statement_node_t*)statement);
        case LS_FOR_STATEMENT        : return BINDER_bindForStatement(me, (ls_for_statement_node_t*)statement);
        case LS_DO_STATEMENT         : return BINDER_bindDoStatement(me, (ls_do_statement_node_t*)statement);
        case LS_WHILE_STATEMENT      : return BINDER_bindWhileStatement(me, (ls_while_statement_node_t*)statement);
        case LS_GOTO_STATEMENT       : return BINDER_bindGotoStatement(me, (ls_goto_statement_node_t*)statement);
        case LS_LABEL_STATEMENT      : return BINDER_bindLabelStatement(me, (ls_label_statement_node_t*)statement);
        case LS_EXIT_STATEMENT       : return BINDER_bindExitStatement(me, (ls_exit_statement_node_t*)statement);
        case LS_CALL_STATEMENT       : return BINDER_bindCallStatement(me, (ls_call_statement_node_t*)statement);
        case LS_EXPRESSION_STATEMENT : return BINDER_bindExpressionStatement(me, (ls_expression_statement_node_t*)statement);

        default:
            ERROR_SPLICE(SUB_BINDER, ERR_INTERNAL, "Unknown statement type in binder '%d'", statement->type)
            me->hasError = true;
        return NULL;
    }
}

tg_ast_node_t *BINDER_bindAssignmentStatement(binder_t *me, ls_assignment_statement_node_t *statement) {

    // bind the reference to the thing we want to assign to
    tg_ast_node_t *target = BINDER_bindExpression(me, statement->exprTarget);

    // bind the value we want to assign
    tg_ast_node_t *value = BINDER_bindExpression(me, statement->exprValue);

    // allocate and return a new node
    tg_assignment_statement_t *node = malloc(sizeof(tg_assignment_statement_t));
    node->base.type = TG_ASSIGNMENT_STATEMENT;
    node->target = target;
    node->value = value;
    return (tg_ast_node_t*)node;
}

tg_ast_node_t *BINDER_bindCallStatement(binder_t *me, ls_call_statement_node_t *statement) {

    // unpack the base reference expression for this call
    ls_reference_expression_node_t *exprCall = (ls_reference_expression_node_t*)statement->exprCall;

    // make sure it has parentheses, very important stuff, thanks microsoft, very cool
    if (exprCall->pcOpenParenthesis == NULL) {
        ERROR_AT(SUB_BINDER, ERR_BD_MISSING_PARENTHESES, me->currentModule->source, SPAN_FromToken(*exprCall->idName), "Calling a subroutine using the Call keyword requires parentheses");
        me->hasError = true;
        return NULL;
    }

    // resolve the internal reference expression into a symbol
    procedure_symbol_t *proc = BINDER_ResolveProcedure(me, exprCall);
    if (proc == NULL) return NULL; // doesnt exist

    // make sure the procedure we found is a subroutine
    // only subroutines are allowed in Call statements
    if (proc->symReturnType != NULL) {
        ERROR_AT(SUB_BINDER, ERR_BD_UNEXPECTED_NON_SUBROUTINE, me->currentModule->source, SPAN_FromToken(*exprCall->idName), "Only subroutines are allowed to be invoked by the Call statement");
        me->hasError = true;
        return NULL;
    }

    // -----------------------------------------------------------------------------------------------------------------
    // make sure the arguments line up
    tg_ast_node_list_t boundArguments = BINDER_bindCallArguments(me, proc, exprCall->lsArguments, SPAN_FromToken(*exprCall->idName));


    // finally: create our call statement
    tg_proc_call_statement_t *node = malloc(sizeof(tg_proc_call_statement_t));
    node->base.type = TG_PROC_CALL_STATEMENT;
    node->procedure = proc;
    node->arguments = boundArguments;
    return (tg_ast_node_t*)node;
}

tg_ast_node_t *BINDER_bindDimStatement(binder_t *me, ls_dim_statement_node_t *statement, tg_ast_node_list_t *boundStatementList) {

    // treat each dim field clause as its own statement
    for (int d = 0; d < statement->lsDimFields.length; ++d) {
        ls_dim_field_clause_node_t *clsDimField = (ls_dim_field_clause_node_t*)statement->lsDimFields.nodes[d];

        // resolve the type symbol for this new local variable
        type_symbol_t *type = BINDER_ResolveAsClause(me, me->currentModule, clsDimField->clsType, BD_RS_ALLOW_ALL_ARRAYS);
        if (type == NULL) continue; // if we couldnt resolve this type, continue with the next field

        // create a new local variable symbol for it
        local_variable_symbol_t *newLocalVariable = malloc(sizeof(local_variable_symbol_t));

        // set up the symbol
        newLocalVariable->base.type = LOCAL_VARIABLE_SYMBOL;
        newLocalVariable->base.declarationSpan = SPAN_FromToken(*clsDimField->idName);
        strcpy(newLocalVariable->base.name, clsDimField->idName->strValue);

        // remember this variables type
        newLocalVariable->symType = type;

        // also give it a new bucket index
        newLocalVariable->bucketIndex = me->currentProcedure->lsBuckets.length;

        // finally try to add it to the list
        if (!BINDER_addLocalVariableToProcedureSymbol(me, me->currentModule, me->currentProcedure, newLocalVariable)) {
            continue; // there was an issue adding this local variable to the procedure scope
        }

        // -------------------------------------------------------------------------------------------------------------
        // now that the variable exists, initialize it

        // create a new list of array ranges
        tg_ast_node_list_t boundRanges = BD_TG_AST_NODE_LIST_Init();

        // if ranges exist
        if (clsDimField->clsType != NULL) {

            // go through all ranges and bind them
            for (int i = 0; i < clsDimField->clsType->lsArrRanges.length; ++i) {
                ls_arr_range_clause_node_t *clsRange = (ls_arr_range_clause_node_t*)clsDimField->clsType->lsArrRanges.nodes[i];

                // if theres no lower bound -> substitute a default zero expression
                if (clsRange->ltLBound == NULL) {
                    BD_TG_AST_NODE_LIST_Add(&boundRanges, BINDER_bindDefaultExpression(me, BINDER_resolveTypeNameFromBuffer(me, "integer")));
                }

                // otherwise if there is a lower bound -> bind it
                else {
                    BD_TG_AST_NODE_LIST_Add(&boundRanges, BINDER_bindExpression(me, clsRange->ltLBound));
                }

                // if theres should always be an upper bound
                BD_TG_AST_NODE_LIST_Add(&boundRanges, BINDER_bindExpression(me, clsRange->ltUBound));
            }
        }

        // when we're done, allocate a new node
        tg_initialize_statement_t *node = malloc(sizeof(tg_initialize_statement_t));
        node->base.type = TG_INITIALIZE_STATEMENT;
        node->variable = newLocalVariable;
        node->preserve = false;
        node->ranges = boundRanges;

        // directly add it to the bound statement list
        // this is the only way we can return multiple bound nodes out of a single unbound node
        BD_TG_AST_NODE_LIST_Add(boundStatementList, (tg_ast_node_t*)node);
    }

    // we dont actually return a node from here
    // instead they will be added directly to the bound statement list which is calling this
    return NULL;
}

tg_ast_node_t *BINDER_bindDoStatement(binder_t *me, ls_do_statement_node_t *statement) {

    // make sure we have either a head conjunction or a tail conjunction
    if (statement->kwHeadConjunction != NULL && statement->kwTailConjunction != NULL) {
        ERROR_AT(SUB_BINDER, ERR_BD_INVALID_DO_LOOP_CONJUNCTIONS, me->currentModule->source, SPAN_FromToken(*statement->kwTailConjunction), "Do Loop cannot be head and tail controlled at the same time");
        me->hasError = true;
        return NULL;
    }

    // figure out if this is head or tail controlled
    token_t *kwConjunction = statement->kwHeadConjunction;
    do_statement_kind_t controlOfDo = DO_STATEMENT_HEAD;
    if (statement->kwTailConjunction != NULL) {
        controlOfDo = DO_STATEMENT_FOOT;
        kwConjunction = statement->kwTailConjunction;
    }

    // figure out what kind of check this loop should do
    do_statement_kind_t kindOfDo = DO_STATEMENT_INFINITE;
    if (kwConjunction != NULL) {
        if (kwConjunction->type == TK_KW_WHILE) {
            kindOfDo = DO_STATEMENT_WHILE;
        }
        else if (kwConjunction->type == TK_KW_UNTIL) {
            kindOfDo = DO_STATEMENT_UNTIL;
        }
    }

    // bind the loop condition
    tg_ast_node_t *condition = NULL;
    if (kindOfDo != DO_STATEMENT_INFINITE) {
        condition = BINDER_bindExpression(me, statement->exprCondition);
    }

    // create a new pair of labels for this loop
    label_symbol_t *continueLabel = BINDER_generateLabel(me, me->currentModule, me->currentProcedure, SPAN_FromToken(*statement->kwDo), "continue_do");
    label_symbol_t *breakLabel = BINDER_generateLabel(me, me->currentModule, me->currentProcedure, SPAN_FromToken(*statement->kwDo), "break_do");

    // push the break label onto the stack
    BD_SYMBOL_LIST_Add(&me->stkDoBreakLabels, (symbol_t*)breakLabel);

    // bind the loop body
    tg_ast_node_list_t boundBody = BINDER_bindStatementList(me, statement->lsBody);

    // pop the break label back off the stack
    BD_SYMBOL_LIST_RemoveLast(&me->stkDoBreakLabels);

    // allocate and return the new node
    tg_do_statement_t *node = malloc(sizeof(tg_do_statement_t));
    node->base.type = TG_DO_STATEMENT;
    node->kindOfDo = kindOfDo;
    node->controlOfDo = controlOfDo;
    node->condition = condition;
    node->statements = boundBody;
    node->continueLabel = continueLabel;
    node->breakLabel = breakLabel;
    return (tg_ast_node_t*)node;
}

tg_ast_node_t *BINDER_bindExitStatement(binder_t *me, ls_exit_statement_node_t *statement) {

    label_symbol_t *exitLabel = NULL;

    // if this is an Exit Function...
    if (statement->kwContainer->type == TK_KW_FUNCTION) {

        // are we even inside a function?
        if (me->currentProcedure->symReturnType == NULL) {
            ERROR_AT(SUB_BINDER, ERR_BD_INVALID_EXIT_CONTAINER, me->currentModule->source, SPAN_FromToken(*statement->kwContainer), "Exit Function cannot be used inside a Subroutine");
            me->hasError = true;
            return NULL;
        }

        // if we are -> use the procedures return label as the exit
        exitLabel = me->currentProcedure->returnLabel;
    }

    // if this is an Exit Sub...
    if (statement->kwContainer->type == TK_KW_SUB) {

        // are we even inside a subroutine?
        if (me->currentProcedure->symReturnType != NULL) {
            ERROR_AT(SUB_BINDER, ERR_BD_INVALID_EXIT_CONTAINER, me->currentModule->source, SPAN_FromToken(*statement->kwContainer), "Exit Sub cannot be used inside a Function");
            me->hasError = true;
            return NULL;
        }

        // if we are -> use the procedures return label as the exit
        exitLabel = me->currentProcedure->returnLabel;
    }

    // if this is an Exit Do...
    if (statement->kwContainer->type == TK_KW_DO) {

        // are we even inside a Do loop?
        if (me->stkDoBreakLabels.length == 0) {
            ERROR_AT(SUB_BINDER, ERR_BD_INVALID_EXIT_CONTAINER, me->currentModule->source, SPAN_FromToken(*statement->kwContainer), "There is no Do loop to Exit out of");
            me->hasError = true;
            return NULL;
        }

        // if we are -> get the break label from the stack
        exitLabel = (label_symbol_t*)me->stkDoBreakLabels.symbols[me->stkDoBreakLabels.length - 1];
    }

    // if this is an Exit For...
    if (statement->kwContainer->type == TK_KW_FOR) {

        // are we even inside a For loop?
        if (me->stkForBreakLabels.length == 0) {
            ERROR_AT(SUB_BINDER, ERR_BD_INVALID_EXIT_CONTAINER, me->currentModule->source, SPAN_FromToken(*statement->kwContainer), "There is no For loop to Exit out of");
            me->hasError = true;
            return NULL;
        }

        // if we are -> get the break label from the stack
        exitLabel = (label_symbol_t*)me->stkForBreakLabels.symbols[me->stkForBreakLabels.length - 1];
    }

    // return this as a goto statement
    tg_goto_statement_t *node = malloc(sizeof(tg_goto_statement_t));
    node->base.type = TG_GOTO_STATEMENT;
    node->target = exitLabel;
    return (tg_ast_node_t*)node;
}

tg_ast_node_t *BINDER_bindExpressionStatement(binder_t *me, ls_expression_statement_node_t *statement) {

    // only reference expressions are allowed to be statements
    if (statement->exprExpression->type != LS_REFERENCE_EXPRESSION) {
        ERROR(SUB_BINDER, ERR_INTERNAL, "only reference expressions should be allowed to be used as statements");
        me->hasError = true;
        return NULL;
    }

    // unpack the base reference expression for this call
    ls_reference_expression_node_t *exprCall = (ls_reference_expression_node_t*)statement->exprExpression;

    // resolve the internal reference expression into a symbol
    procedure_symbol_t *proc = BINDER_ResolveProcedure(me, exprCall);
    if (proc == NULL) return NULL; // doesnt exist

    // if this is a subroutine, make sure it doesnt have parentheses
    if (proc->symReturnType == NULL && exprCall->pcOpenParenthesis != NULL) {
        ERROR_AT(SUB_BINDER, ERR_BD_ILLEGAL_PARENTHESES, me->currentModule->source, SPAN_FromToken(*exprCall->idName), "Calling a subroutine without the Call keyword must not use any parentheses");
        me->hasError = true;
        return NULL;
    }

    // if its a function i really do not care

    // -----------------------------------------------------------------------------------------------------------------
    // make sure the arguments line up
    tg_ast_node_list_t boundArguments = BINDER_bindCallArguments(me, proc, exprCall->lsArguments, SPAN_FromToken(*exprCall->idName));

    // finally: create our call statement
    tg_proc_call_statement_t *node = malloc(sizeof(tg_proc_call_statement_t));
    node->base.type = TG_PROC_CALL_STATEMENT;
    node->procedure = proc;
    node->arguments = boundArguments;
    return (tg_ast_node_t*)node;
}

tg_ast_node_t *BINDER_bindForStatement(binder_t *me, ls_for_statement_node_t *statement) {

    symbol_t *iterator = NULL;

    // try to look up the given identifier
    int idx = BD_SYMBOL_LIST_Find(&me->currentProcedure->lsBuckets, statement->idIterator->strValue);
    if (idx != -1) {

        // if it exists -> find out its type
        iterator = me->currentProcedure->lsBuckets.symbols[idx];
        type_symbol_t *type = NULL;

        if (iterator->type == PARAMETER_SYMBOL) {
            type = ((parameter_symbol_t*)iterator)->symType;
        }
        else if (iterator->type == LOCAL_VARIABLE_SYMBOL) {
            type = ((local_variable_symbol_t*)iterator)->symType;
        }

        // make sure its some sort of integer or variant
        if (
            !BINDER_areTypesEqual(type, (type_symbol_t*)me->programUnit->lsBuiltinTypes.symbols[IDX_BUILTIN_VARIANT]) &&
            !BINDER_areTypesEqual(type, BINDER_resolveTypeNameFromBuffer(me, "byte")) &&
            !BINDER_areTypesEqual(type, BINDER_resolveTypeNameFromBuffer(me, "integer")) &&
            !BINDER_areTypesEqual(type, BINDER_resolveTypeNameFromBuffer(me, "long")) &&
            !BINDER_areTypesEqual(type, BINDER_resolveTypeNameFromBuffer(me, "longlong"))
        ) {
            ERROR_AT(SUB_BINDER, ERR_BD_INVALID_ITERATOR_TYPE, me->currentModule->source, SPAN_FromToken(*statement->idIterator), "Iterator variable must be of type Variant or any of the Integer types");
            me->hasError = true;
            return NULL;
        }
    }

    // if the iterator doesnt exist yet -> create a new symbol
    else {

        // create a new local variable symbol for it
        local_variable_symbol_t *newIterator = malloc(sizeof(local_variable_symbol_t));

        // set up the symbol
        newIterator->base.type = LOCAL_VARIABLE_SYMBOL;
        newIterator->base.declarationSpan = SPAN_FromToken(*statement->idIterator);
        strcpy(newIterator->base.name, statement->idIterator->strValue);

        // remember this variables type
        newIterator->symType = (type_symbol_t*)me->programUnit->lsBuiltinTypes.symbols[IDX_BUILTIN_VARIANT];

        // also give it a new bucket index
        newIterator->bucketIndex = me->currentProcedure->lsBuckets.length;

        // add it to the procedures scope
        if (!BINDER_addLocalVariableToProcedureSymbol(me, me->currentModule, me->currentProcedure, newIterator)) {
            return NULL; // there was an issue adding this local variable to the procedure scope
        }

        iterator = (symbol_t*)newIterator;
    }

    // bind the starting position
    tg_ast_node_t *startingPos = BINDER_bindExpression(me, statement->exprStart);

    // create an initializer
    tg_reference_expression_t *iteratorReference = malloc(sizeof(tg_reference_expression_t));
    iteratorReference->base.type = TG_REFERENCE_EXPRESSION;
    iteratorReference->linkSymbol = iterator;

    tg_assignment_statement_t *initializer = malloc(sizeof(tg_assignment_statement_t));
    initializer->base.type = TG_ASSIGNMENT_STATEMENT;
    initializer->target = (tg_ast_node_t*)iteratorReference;
    initializer->value = startingPos;

    // bind the ending position
    tg_ast_node_t *endingPos = BINDER_bindExpression(me, statement->exprEnd);

    // bind the step
    tg_ast_node_t *stepSize = NULL;

    // if we have a step size -> bind it
    if (statement->exprStep != NULL) {
        stepSize = BINDER_bindExpression(me, statement->exprStep);
    }

    // otherwise, create a constant literal 1 expression
    else {
        stepSize = BINDER_createIntegerLiteralExpression(me, 1);
    }

    // create new labels for this loop
    label_symbol_t *continueLabel = BINDER_generateLabel(me, me->currentModule, me->currentProcedure, SPAN_FromToken(*statement->kwFor), "continue_for");
    label_symbol_t *breakLabel = BINDER_generateLabel(me, me->currentModule, me->currentProcedure, SPAN_FromToken(*statement->kwFor), "break_for");

    // push the for break label onto the stack
    BD_SYMBOL_LIST_Add(&me->stkForBreakLabels, (symbol_t*)breakLabel);

    // bind the loop body
    tg_ast_node_list_t boundBody = BINDER_bindStatementList(me, statement->lsBody);

    // pop the break label back off the stack
    BD_SYMBOL_LIST_RemoveLast(&me->stkForBreakLabels);

    // allocate and return a new node
    tg_for_statement_t *node = malloc(sizeof(tg_for_statement_t));
    node->base.type = TG_FOR_STATEMENT;
    node->initializer = (tg_ast_node_t*)initializer;
    node->upperBound = endingPos;
    node->step = stepSize;
    node->continueLabel = continueLabel;
    node->breakLabel = breakLabel;
    node->statements = boundBody;
    return (tg_ast_node_t*)node;
}

tg_ast_node_t *BINDER_bindGotoStatement(binder_t *me, ls_goto_statement_node_t *statement) {

    // resolve the label we want to go to
    label_symbol_t *target = BINDER_ResolveLabelName(me, me->currentModule, me->currentProcedure, statement->idLabel);

    // allocate and return a new node
    tg_goto_statement_t *node = malloc(sizeof(tg_goto_statement_t));
    node->base.type = TG_GOTO_STATEMENT;
    node->target = target;
    return (tg_ast_node_t*)node;
}

tg_ast_node_t *BINDER_bindIfStatement(binder_t *me, ls_if_statement_node_t *statement) {

    // create a list for all our conditions
    tg_ast_node_list_t conditions = BD_TG_AST_NODE_LIST_Init();

    // create a list for all our bodies
    tg_ast_node_list_list_t bodies = BD_TG_AST_NODE_LIST_LIST_Init();

    for (int i = 0; i < statement->lsConditionals.length; ++i) {
        ls_conditional_clause_node_t *clsConditional = (ls_conditional_clause_node_t*)statement->lsConditionals.nodes[i];

        // bind the condition
        tg_ast_node_t *condition = BINDER_bindExpression(me, clsConditional->exprCondition);
        BD_TG_AST_NODE_LIST_Add(&conditions, condition);

        // bind the body
        tg_ast_node_list_t body = BINDER_bindStatementList(me, clsConditional->lsStatements);
        BD_TG_AST_NODE_LIST_LIST_Add(&bodies, body);
    }

    // create a list for the else clause
    tg_ast_node_list_t elseBody = BD_TG_AST_NODE_LIST_Init();

    // if there is an else block, bind it
    if (statement->clsElse != NULL) {
        ls_else_clause_node_t *clsElse = (ls_else_clause_node_t*)statement->clsElse;

        // bind the body
        elseBody = BINDER_bindStatementList(me, clsElse->lsStatements);
    }

    // allocate and return a new node
    tg_if_statement_t *node = malloc(sizeof(tg_if_statement_t));
    node->base.type = TG_IF_STATEMENT;
    node->branchConditions = conditions;
    node->branchStatements = bodies;
    node->elseStatements = elseBody;
    return (tg_ast_node_t*)node;
}

tg_ast_node_t *BINDER_bindLabelStatement(binder_t *me, ls_label_statement_node_t *statement) {

    // resolve the label this is referencing
    label_symbol_t *target = BINDER_ResolveLabelName(me, me->currentModule, me->currentProcedure, statement->idLabel);

    // allocate and return a new node
    tg_label_statement_t *node = malloc(sizeof(tg_label_statement_t));
    node->base.type = TG_LABEL_STATEMENT;
    node->me = target;
    return (tg_ast_node_t*)node;

}

tg_ast_node_t *BINDER_bindRedimStatement(binder_t *me, ls_redim_statement_node_t *statement) {

    // should we preserve redimmed data?
    bool preserve = false;
    if (statement->kwPreserve != NULL) {
        preserve = true;
    }

    // go through each redim field
    for (int d = 0; d < statement->lsDimFields.length; ++d) {

    }
}

tg_ast_node_t *BINDER_bindSelectStatement(binder_t *me, ls_select_statement_node_t *statement) {

}

tg_ast_node_t *BINDER_bindWhileStatement(binder_t *me, ls_while_statement_node_t *statement) {

}

tg_ast_node_list_t BINDER_bindCallArguments(binder_t *me, procedure_symbol_t *target, ls_ast_node_list_t arguments, span_t errorLocation) {

    // create a new list for us to put these arguments
    tg_ast_node_list_t boundArguments = BD_TG_AST_NODE_LIST_Init();

    // -----------------------------------------------------------------------------------------------------------------
    // make sure the arguments line up

    // count how many arguments are needed / allowed
    int minNumberOfArgs = 0;
    int maxNumberOfArgs = 0;

    for (int i = 0; i < target->lsParameters.length; ++i) {
        parameter_symbol_t *param = (parameter_symbol_t*)target->lsParameters.symbols[i];

        // non optional parameters count towards min and max args
        if (!param->isOptional) {
            minNumberOfArgs++;
            maxNumberOfArgs++;
        }

        // optional parameters count towards max args
        if (param->isOptional) {
            maxNumberOfArgs++;
        }
    }

    // if this procedure is variadic, decrease the minimum by 1
    // -> a variadic list can also be of length 0
    if (target->isVariadic) minNumberOfArgs--;

    // also if its variadic, allow as many args as i think is reasonable
    if (target->isVariadic) maxNumberOfArgs = MAX_VARIADIC_ARGS;

    // ---------------------------------------------------------
    // is the given amount of arguments somewhere in that range?

    // did we get too few arguments
    if (arguments.length < minNumberOfArgs) {
        ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_INVALID_NUMBER_OF_ARGUMENTS, me->currentModule->source, errorLocation, "Procedure '%s', requires at least %d arguments to be called", target->base.name, minNumberOfArgs);
        me->hasError = true;
        return boundArguments;
    }

    // did we get too many arguments
    if (arguments.length > maxNumberOfArgs) {
        ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_INVALID_NUMBER_OF_ARGUMENTS, me->currentModule->source, errorLocation, "Procedure '%s', allows at most %d arguments to be called", target->base.name, maxNumberOfArgs);
        me->hasError = true;
        return boundArguments;
    }

    // now: bind the arguments
    // -----------------------

    // iterate through all arguments
    for (int i = 0; i < arguments.length; ++i) {

        ls_ast_node_t *arg = arguments.nodes[i];
        tg_ast_node_t *boundArgument = NULL;

        // figure out the type of this argument for later
        type_symbol_t *paramType = NULL;

        // if this is a variadic call and our argument is in the middle of nowhere
        // use the type of the variadic parameter
        if (target->isVariadic && i >= target->lsParameters.length-1) {
            parameter_symbol_t *param = (parameter_symbol_t*)target->lsParameters.symbols[target->lsParameters.length-1];

            // variadic parameters are always of an array type
            // -> use the element type
            paramType = param->symType->symSubType;
        }

        // otherwise, if this is a plain old argument
        // -> simply look up its type
        else {
            parameter_symbol_t *param = (parameter_symbol_t*)target->lsParameters.symbols[i];
            paramType = param->symType;
        }

        // has this argument been given?
        if (arg == NULL) {

            // if not, check if its actually allowed to be omitted
            if (!target->isVariadic ||
                (target->isVariadic && i < target->lsParameters.length-1)) {
                parameter_symbol_t *param = (parameter_symbol_t*)target->lsParameters.symbols[i];
                if (!param->isOptional) {
                    ERROR_SPLICE_AT(SUB_BINDER, ERR_BD_INVALID_NUMBER_OF_ARGUMENTS, me->currentModule->source, errorLocation, "Parameter '%s' (parameter %d) is not allowed to be omitted", param->base.name, i);
                    me->hasError = true;
                    continue;
                }
            }

            // if it can be omitted, load a default value instead
            boundArgument = BINDER_bindDefaultExpression(me, paramType);

            // add it to the list and move on
            BD_TG_AST_NODE_LIST_Add(&boundArguments, boundArgument);
            continue;
        }

        // otherwise: if we do have an argument here
        // -> bind it
        boundArgument = BINDER_bindExpression(me, arg);

        // if our parameter is not of type variant
        // -> ensure it is at runtime
        if (!BINDER_areTypesEqual(paramType, (type_symbol_t*)me->programUnit->lsBuiltinTypes.symbols[IDX_BUILTIN_VARIANT])) {
            boundArgument = BINDER_bindEnsureTypeExpression(me, paramType, boundArgument);
        }

        // add it to the list and move on
        BD_TG_AST_NODE_LIST_Add(&boundArguments, boundArgument);
    }

    return boundArguments;
}

tg_ast_node_t *BINDER_bindExpression(binder_t *me, ls_ast_node_t *expression) {
    return BINDER_bindDefaultExpression(me, (type_symbol_t*)me->programUnit->lsBuiltinTypes.symbols[IDX_BUILTIN_VARIANT]);
}

tg_ast_node_t *BINDER_bindDefaultExpression(binder_t *me, type_symbol_t *type) {
    tg_default_expression_t *node = malloc(sizeof(tg_default_expression_t));
    node->base.type == TG_DEFAULT_EXPRESSION;
    node->type = type;
    return (tg_ast_node_t*)node;
}

tg_ast_node_t *BINDER_bindEnsureTypeExpression(binder_t *me, type_symbol_t *type, tg_ast_node_t *expression) {
    tg_ensure_type_expression_t *node = malloc(sizeof(tg_ensure_type_expression_t));
    node->base.type = TG_ENSURE_TYPE_EXPRESSION;
    node->expression = expression;
    node->targetType = type;
    return (tg_ast_node_t*)node;
}

tg_ast_node_t *BINDER_createIntegerLiteralExpression(binder_t *me, int16_t value) {
    tg_literal_expression_t *node = malloc(sizeof(tg_literal_expression_t));
    node->base.type = TG_LITERAL_EXPRESSION;

    int16_t *num = malloc(sizeof(int16_t));
    *num = value;

    node->value = num;
    node->literalType = BINDER_resolveTypeNameFromBuffer(me, "integer");
    return (tg_ast_node_t*)node;
}