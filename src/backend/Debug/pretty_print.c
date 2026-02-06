//
// Created by ds on 12/23/25.
//
#include "pretty_print.h"
#include "ansi.h"

#include <stdio.h>

#include "AST/Loose/ls_ast.h"
#include "AST/Loose/Clauses/arr_range_clause.h"
#include "AST/Loose/Clauses/conditional_clause.h"
#include "AST/Loose/Clauses/dim_field_clause.h"
#include "AST/Loose/Clauses/parameter_clause.h"
#include "AST/Loose/Clauses/type_field_clause.h"
#include "AST/Loose/Expressions/binary_expression.h"
#include "AST/Loose/Expressions/literal_expression.h"
#include "AST/Loose/Expressions/parenthesized_expression.h"
#include "AST/Loose/Expressions/reference_expression.h"
#include "AST/Loose/Expressions/unary_expression.h"
#include "AST/Loose/Members/function_member.h"
#include "AST/Loose/Members/module_member.h"
#include "AST/Loose/Members/sub_member.h"
#include "AST/Loose/Members/type_member.h"
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
#include "AST/Tight/program_unit.h"
#include "AST/Tight/Symbols/local_variable_symbol.h"
#include "AST/Tight/Symbols/module_symbol.h"
#include "AST/Tight/Symbols/parameter_symbol.h"
#include "AST/Tight/Symbols/procedure_symbol.h"
#include "AST/Tight/Symbols/type_field_symbol.h"
#include "AST/Tight/Symbols/type_symbol.h"

char DBG_INDENT_BUFFER[256];
int DBG_INDENT_LENGTH;

void DBG_PRETTY_PRINT_Print_TokenList(token_list_t tokens) {
    printf("List of tokens:\n");

    for (int i = 0; i < tokens.length; i++) {
        if (tokens.tokens[i].type == TK_LT_STRING) {
            printf("[%d]: " BYEL "%s," CYN " '%s'" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, tokens.tokens[i].line, tokens.tokens[i].column);
        } else if (tokens.tokens[i].type == TK_LT_BOOLEAN) {
            printf("[%d]: " BYEL "%s," CYN " '%s'" GRN " %d" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, *((bool*)tokens.tokens[i].value), tokens.tokens[i].line, tokens.tokens[i].column);
        } else if (tokens.tokens[i].type == TK_LT_NUMBER) {
            if (tokens.tokens[i].numberValueType == NUMBER_VALUE_BYTE)
                printf("[%d]: " BYEL "%s," CYN " '%s'" GRN " NUMBER_VALUE_BYTE %u" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, *((uint8_t*)tokens.tokens[i].value), tokens.tokens[i].line, tokens.tokens[i].column);
            else if (tokens.tokens[i].numberValueType == NUMBER_VALUE_INT)
                printf("[%d]: " BYEL "%s," CYN " '%s'" GRN " NUMBER_VALUE_INT %d" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, *((int16_t*)tokens.tokens[i].value), tokens.tokens[i].line, tokens.tokens[i].column);
            else if (tokens.tokens[i].numberValueType == NUMBER_VALUE_LONG)
                printf("[%d]: " BYEL "%s," CYN " '%s'" GRN " NUMBER_VALUE_LONG %d" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, *((int32_t*)tokens.tokens[i].value), tokens.tokens[i].line, tokens.tokens[i].column);
            else if (tokens.tokens[i].numberValueType == NUMBER_VALUE_LONG_LONG)
                printf("[%d]: " BYEL "%s," CYN " '%s'" GRN " NUMBER_VALUE_LONG_LONG %ld" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, *((int64_t*)tokens.tokens[i].value), tokens.tokens[i].line, tokens.tokens[i].column);
            else if (tokens.tokens[i].numberValueType == NUMBER_VALUE_SINGLE)
                printf("[%d]: " BYEL "%s," CYN " '%s'" GRN " NUMBER_VALUE_SINGLE %f" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, *((float*)tokens.tokens[i].value), tokens.tokens[i].line, tokens.tokens[i].column);
            else if (tokens.tokens[i].numberValueType == NUMBER_VALUE_DOUBLE)
                printf("[%d]: " BYEL "%s," CYN " '%s'" GRN " NUMBER_VALUE_DOUBLE %lf" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, *((double*)tokens.tokens[i].value), tokens.tokens[i].line, tokens.tokens[i].column);
            else if (tokens.tokens[i].numberValueType == NUMBER_VALUE_CURRENCY)
                printf("[%d]: " BYEL "%s," CYN " '%s'" GRN " NUMBER_VALUE_CURRENCY %ld" CRESET " (L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, *((int64_t*)tokens.tokens[i].value), tokens.tokens[i].line, tokens.tokens[i].column);

        }
        else if (tokens.tokens[i].type == TK_EOS) {
            if (tokens.tokens[i].strValue == NULL)
                printf("[%d]: " BYEL "%s " CRESET "(L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].line, tokens.tokens[i].column);
            else
                printf("[%d]: " BYEL "%s" CYN " '%s' " CRESET "(L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].strValue, tokens.tokens[i].line, tokens.tokens[i].column);
        }
        else {
            printf("[%d]: " BYEL "%s " CRESET "(L: %d, C: %d)\n", i, TOKEN_TYPE_STRING[tokens.tokens[i].type], tokens.tokens[i].line, tokens.tokens[i].column);
        }
    }
}

void DBG_PRETTY_PRINT_Print_TokenList_AsSource(token_list_t tokens) {
    int indent = 0;
    printf("Highlighted source reconstructed from the token list:\n");

    for (int i = 0; i < tokens.length; ++i) {
        token_t token = tokens.tokens[i];

        if (token.type == TK_KW_SUB) indent += 2;
        if (token.type == TK_KW_FUNCTION) indent += 2;
        if (token.type == TK_KW_END) indent -= 4;
        if (token.type == TK_KW_EXIT) indent -= 2;
        if (token.type == TK_KW_FOR) indent += 2;
        if (token.type == TK_KW_NEXT) indent -= 2;

        if (token.type == TK_KW_END || token.type == TK_KW_NEXT) {
            printf("\r");
            for (int j = 0; j < indent; ++j) {
                printf(" ");
            }
        }

        if (token.type == TK_EOS) {
            printf("\n");
            for (int j = 0; j < indent; ++j) {
                printf(" ");
            }
            continue;
        }

        if (token.type == TK_ERROR) {
            printf(REDB "<error>" CRESET " ");
            continue;
        }

        if (TOKEN_TYPE_STRING[token.type][3] == 'K' && TOKEN_TYPE_STRING[token.type][4] == 'W') {
            printf(BRED "%s ", token.strValue);
            continue;
        }

        if (token.type == TK_IDENTIFIER) {
            printf(YEL "%s ", token.strValue);
            continue;
        }

        if (token.type == TK_LT_STRING) {
            printf(GRN "\"%s\" ", token.strValue);
            continue;
        }

        if (token.type == TK_LT_BOOLEAN) {
            printf(CYN "%s ", token.strValue);
            continue;
        }

        if (token.type == TK_LT_NUMBER) {
            printf(CYN "%s ", token.strValue);
            continue;
        }

        if (token.type == TK_PC_OPEN_PARENTHESIS) {
            printf(CRESET "(");
            continue;
        }

        if (token.type == TK_PC_CLOSED_PARENTHESIS) {
            printf(CRESET ") ");
            continue;
        }

        if (token.type == TK_PC_COMMA) {
            printf(CRESET ", ");
            continue;
        }

        if (token.type == TK_OP_PLUS) {
            printf(CRESET "+ ");
            continue;
        }

        if (token.type == TK_OP_MINUS) {
            printf(CRESET "- ");
            continue;
        }

        if (token.type == TK_OP_STAR) {
            printf(CRESET "* ");
            continue;
        }

        if (token.type == TK_OP_SLASH) {
            printf(CRESET "/ ");
            continue;
        }

        if (token.type == TK_OP_AND) {
            printf(CRESET "& ");
            continue;
        }

        if (token.type == TK_OP_EQUALS) {
            printf(CRESET "= ");
            continue;
        }
    }
}

void DBG_PRETTY_PRINT_Print_LSAstNode(ls_ast_node_t *me, bool finalEntry) {
    INDENT()

    if (me == NULL) {
        NULL_NODE()
        return;
    }

    switch (me->type) {
        NODE(LS_FUNCTION_MEMBER)
            FIELD("Public")
            VALUE_SET(((ls_function_member_node_t*)me)->kwPublic)

            FIELD("Private")
            VALUE_SET(((ls_function_member_node_t*)me)->kwPrivate)

            FIELD("Friend")
            VALUE_SET(((ls_function_member_node_t*)me)->kwFriend)

            FIELD("Static")
            VALUE_SET(((ls_function_member_node_t*)me)->kwStatic)

            FIELD("Name")
            VALUE(((ls_function_member_node_t*)me)->idName)

            FIELD("Parameters")
            SUBNODES(((ls_function_member_node_t*)me)->lsParameters, false)

            FIELD("Return Type")
            SUBNODE(((ls_function_member_node_t*)me)->clsReturnType, false)

            FIELD_FINAL("Body")
            SUBNODES(((ls_function_member_node_t*)me)->lsFunctionBody, true)
        END_NODE()

        NODE(LS_SUBROUTINE_MEMBER)
            FIELD("Public")
            VALUE_SET(((ls_sub_member_node_t*)me)->kwPublic)

            FIELD("Private")
            VALUE_SET(((ls_sub_member_node_t*)me)->kwPrivate)

            FIELD("Friend")
            VALUE_SET(((ls_sub_member_node_t*)me)->kwFriend)

            FIELD("Static")
            VALUE_SET(((ls_sub_member_node_t*)me)->kwStatic)

            FIELD("Name")
            VALUE(((ls_sub_member_node_t*)me)->idName)

            FIELD("Parameters")
            SUBNODES(((ls_sub_member_node_t*)me)->lsParameters, false)

            FIELD_FINAL("Body")
            SUBNODES(((ls_sub_member_node_t*)me)->lsSubBody, true)
        END_NODE()

        NODE(LS_MODULE_MEMBER)
            FIELD_FINAL("Name")
            VALUE(((ls_module_member_node_t*)me)->idModuleName)
        END_NODE()

        NODE(LS_TYPE_MEMBER)
            FIELD("Name")
            VALUE(((ls_type_member_node_t*)me)->idTypeName)

            FIELD_FINAL("Fields")
            SUBNODES(((ls_type_member_node_t*)me)->lsFields, true)
        END_NODE()

        NODE(LS_TYPE_FIELD_CLAUSE)
            FIELD("Name")
            VALUE(((ls_type_field_clause_node_t*)me)->idFieldName)

            FIELD_FINAL("Type")
            SUBNODE(((ls_type_field_clause_node_t*)me)->clsType, true)
        END_NODE()

        NODE(LS_AS_CLAUSE)
            FIELD("OpenParenthesis")
            VALUE_SET(((ls_as_clause_node_t*)me)->pcOpenParenthesis)

            FIELD("Ranges")
            SUBNODES(((ls_as_clause_node_t*)me)->lsArrRanges, false)

            FIELD("ClosedParenthesis")
            VALUE_SET(((ls_as_clause_node_t*)me)->pcClosedParenthesis)

            FIELD_FINAL("Type")
            VALUE(((ls_as_clause_node_t*)me)->idType)
        END_NODE()

        NODE(LS_ARR_RANGE_CLAUSE)
            FIELD("Lower Bound")
            SUBNODE(((ls_arr_range_clause_node_t*)me)->ltLBound, false)

            FIELD_FINAL("Upper Bound")
            SUBNODE(((ls_arr_range_clause_node_t*)me)->ltUBound, true)
        END_NODE()

        NODE(LS_PARAMETER_CLAUSE)
            FIELD("Optional")
            VALUE_SET(((ls_parameter_clause_node_t*)me)->kwOptional)

            FIELD("ByVal")
            VALUE_SET(((ls_parameter_clause_node_t*)me)->kwByVal)

            FIELD("ByRef")
            VALUE_SET(((ls_parameter_clause_node_t*)me)->kwByRef)

            FIELD("ParamArray")
            VALUE_SET(((ls_parameter_clause_node_t*)me)->kwParamArray)

            FIELD("Name")
            VALUE(((ls_parameter_clause_node_t*)me)->idParamName)

            FIELD("Type")
            SUBNODE(((ls_parameter_clause_node_t*)me)->clsType, false)

            FIELD_FINAL("Default Value")
            SUBNODE(((ls_parameter_clause_node_t*)me)->expDefaultValue, true)
        END_NODE()

        NODE(LS_DIM_STATEMENT)
            FIELD_FINAL("Fields")
            SUBNODES(((ls_dim_statement_node_t*)me)->lsDimFields, true)
        END_NODE()

        NODE(LS_REDIM_STATEMENT)
            FIELD("Preserve")
            VALUE_SET(((ls_redim_statement_node_t*)me)->kwPreserve)

            FIELD_FINAL("Fields")
            SUBNODES(((ls_redim_statement_node_t*)me)->lsDimFields, true)
        END_NODE()

        NODE(LS_DIM_FIELD_CLAUSE)
            FIELD("Name")
            VALUE(((ls_dim_field_clause_node_t*)me)->idName)

            FIELD_FINAL("Type")
            SUBNODE(((ls_dim_field_clause_node_t*)me)->clsType, true)
        END_NODE()

        NODE(LS_REFERENCE_EXPRESSION)
            FIELD("Base")
            SUBNODE(((ls_reference_expression_node_t*)me)->exprBase, false)

            FIELD("LinkName")
            VALUE(((ls_reference_expression_node_t*)me)->idName)

            FIELD("OpenParenthesis")
            VALUE_SET(((ls_reference_expression_node_t*)me)->pcOpenParenthesis)

            FIELD("ClosedParenthesis")
            VALUE_SET(((ls_reference_expression_node_t*)me)->pcClosedParenthesis)

            FIELD_FINAL("Arguments")
            SUBNODES(((ls_reference_expression_node_t*)me)->lsArguments, true)
        END_NODE()

        NODE(LS_EXPRESSION_STATEMENT)
            FIELD_FINAL("Expression")
            SUBNODE(((ls_expression_statement_node_t*)me)->exprExpression, true);
        END_NODE()

        NODE(LS_ASSIGNMENT_STATEMENT)
            FIELD("Let")
            VALUE_SET(((ls_assignment_statement_node_t*)me)->kwLet)

            FIELD("Set")
            VALUE_SET(((ls_assignment_statement_node_t*)me)->kwSet)

            FIELD("Target")
            SUBNODE(((ls_assignment_statement_node_t*)me)->exprTarget, false)

            FIELD_FINAL("Value")
            SUBNODE(((ls_assignment_statement_node_t*)me)->exprValue, true)
        END_NODE()

        NODE(LS_LITERAL_EXPRESSION)
            FIELD_FINAL("Literal")
            VALUE(((ls_literal_expression_node_t*)me)->ltLiteral)
        END_NODE()

        NODE(LS_IF_STATEMENT)
            FIELD("Conditionals")
            SUBNODES(((ls_if_statement_node_t*)me)->lsConditionals, false)

            FIELD_FINAL("Else")
            SUBNODE(((ls_if_statement_node_t*)me)->clsElse, true)
        END_NODE()

        NODE(LS_CONDITIONAL_CLAUSE)
            FIELD("ConditionalStyle")
            VALUE(((ls_conditional_clause_node_t*)me)->kwConditional)

            FIELD("Condition")
            SUBNODE(((ls_conditional_clause_node_t*)me)->exprCondition, false)

            FIELD_FINAL("Statements")
            SUBNODES(((ls_conditional_clause_node_t*)me)->lsStatements, true)
        END_NODE()

        NODE(LS_ELSE_CLAUSE)
            FIELD_FINAL("Statements")
            SUBNODES(((ls_else_clause_node_t*)me)->lsStatements, true)
        END_NODE()

        NODE(LS_SELECT_STATEMENT)
            FIELD("Test")
            SUBNODE(((ls_select_statement_node_t*)me)->exprTest, false)

            FIELD("Cases")
            SUBNODES(((ls_select_statement_node_t*)me)->lsConditionals, false)

            FIELD_FINAL("Case Else")
            SUBNODE(((ls_select_statement_node_t*)me)->clsElse, true)
        END_NODE()

        NODE(LS_GOTO_STATEMENT)
            FIELD_FINAL("Label")
            VALUE(((ls_goto_statement_node_t*)me)->idLabel)
        END_NODE()

        NODE(LS_LABEL_STATEMENT)
            FIELD_FINAL("Label")
            VALUE(((ls_label_statement_node_t*)me)->idLabel)
        END_NODE()

        NODE(LS_FOR_STATEMENT)
            FIELD("Iterator")
            VALUE(((ls_for_statement_node_t*)me)->idIterator)

            FIELD("Start")
            SUBNODE(((ls_for_statement_node_t*)me)->exprStart, false)

            FIELD("End")
            SUBNODE(((ls_for_statement_node_t*)me)->exprEnd, false)

            FIELD("Step")
            SUBNODE(((ls_for_statement_node_t*)me)->exprStep, false)

            FIELD_FINAL("Body")
            SUBNODES(((ls_for_statement_node_t*)me)->lsBody, true)
        END_NODE()

        NODE(LS_WHILE_STATEMENT)
            FIELD("Condition")
            SUBNODE(((ls_while_statement_node_t*)me)->exprCondition, false)

            FIELD_FINAL("Body")
            SUBNODES(((ls_while_statement_node_t*)me)->lsBody, true)
        END_NODE()

        NODE(LS_DO_STATEMENT)
            FIELD("HeadConjunction")
            VALUE(((ls_do_statement_node_t*)me)->kwHeadConjunction)

            FIELD("Condition")
            SUBNODE(((ls_do_statement_node_t*)me)->exprCondition, false)

            FIELD("Body")
            SUBNODES(((ls_do_statement_node_t*)me)->lsBody, false)

            FIELD_FINAL("TailConjunction")
            VALUE(((ls_do_statement_node_t*)me)->kwTailConjunction)
        END_NODE()

        NODE(LS_EXIT_STATEMENT)
            FIELD_FINAL("Container")
            VALUE(((ls_exit_statement_node_t*)me)->kwContainer)
        END_NODE()

        NODE(LS_UNARY_EXPRESSION)
            FIELD("Operator")
            VALUE(((ls_unary_expression_node_t*)me)->opOperator)

            FIELD_FINAL("Operand")
            SUBNODE(((ls_unary_expression_node_t*)me)->exprOperand, true)
        END_NODE()

        NODE(LS_BINARY_EXPRESSION)
            FIELD("Operator")
            VALUE(((ls_binary_expression_node_t*)me)->opOperator)

            FIELD("LeftOperand")
            SUBNODE(((ls_binary_expression_node_t*)me)->exprLeft, false)

            FIELD_FINAL("RightOperand")
            SUBNODE(((ls_binary_expression_node_t*)me)->exprRight, true)
        END_NODE()

        NODE(LS_PARENTHESIZED_EXPRESSION)
            FIELD_FINAL("Inner")
            SUBNODE(((ls_parenthesized_expression_node_t*)me)->exprInner, true)
        END_NODE()

        NODE(LS_CALL_STATEMENT)
            FIELD_FINAL("Reference")
            SUBNODE(((ls_call_statement_node_t*)me)->exprCall, true)
        END_NODE()

        default:;
    }
}

void DBG_PRETTY_PRINT_Print_LSAstNode_List(ls_ast_node_list_t me) {
    for (int i = 0; i < me.length; ++i) {
        DBG_PRETTY_PRINT_Print_LSAstNode(me.nodes[i], i == me.length - 1);
    }
}

void DBG_PRETTY_PRINT_Print_ProgramUnit(program_unit_t *me) {
    printf("\n");
    printf( WHTB BBLK "[Program Unit]\n" CRESET);

    printf(CYNB BBLK "[[Builtin Types]]\n" CRESET);
    DBG_PRETTY_PRINT_Print_SymbolList(me->lsBuiltinTypes);

    printf(CYNB BBLK "[[Array Types]]\n" CRESET);
    DBG_PRETTY_PRINT_Print_SymbolList(me->lsArrayTypes);

    printf(CYNB BBLK "[[Modules]]\n" CRESET);
    DBG_PRETTY_PRINT_Print_SymbolList(me->lsModules);
}

void DBG_PRETTY_PRINT_Print_Symbol(symbol_t *me, bool finalEntry) {
    INDENT()

    switch (me->type) {
        SYMBOL(UNKNOWN_SYMBOL)
            FIELD_FINAL("Name")
            VALUE_STR(me->name)
        END_SYMBOL()

        SYMBOL(MODULE_SYMBOL)

            FIELD("Name")
            VALUE_STR(me->name)

            FIELD("Types")
            SUBSYMBOLS(((module_symbol_t*)me)->lsTypes, false)

            FIELD_FINAL("Procedures")
            SUBSYMBOLS(((module_symbol_t*)me)->lsProcedures, true)

        END_SYMBOL()

        SYMBOL(TYPE_SYMBOL)

            FIELD("Name")
            VALUE_STR(me->name)

            if (((type_symbol_t*)me)->typeOfType == TYPE_BUILTIN) {
                FIELD_FINAL("Kind")
                VALUE_SYM(TYPE_BUILTIN)
            }

            if (((type_symbol_t*)me)->typeOfType == TYPE_ARRAY) {
                FIELD("Kind")
                VALUE_SYM(TYPE_ARRAY)

                FIELD("Dimensions")
                VALUE_NUM(((type_symbol_t*)me)->numArrayDimensions)

                FIELD_FINAL("SubType")
                SUBSYMBOL(((type_symbol_t*)me)->symSubType, true)
            }

            if (((type_symbol_t*)me)->typeOfType == TYPE_USER_DEFINED) {
                FIELD("Kind")
                VALUE_SYM(TYPE_USER_DEFINED)

                FIELD_FINAL("Fields")
                SUBSYMBOLS(((type_symbol_t*)me)->lsFields, true)
            }

        END_SYMBOL()

        SYMBOL(TYPE_FIELD_SYMBOL)

            FIELD("Name")
            VALUE_STR(me->name)

            FIELD_FINAL("Type")
            SUBSYMBOL(((type_field_symbol_t*)me)->symType, true)

        END_SYMBOL()

        SYMBOL(PROCEDURE_SYMBOL)

            FIELD("Name")
            VALUE_STR(me->name)

            FIELD("ProcedureID")
            VALUE_NUM(((procedure_symbol_t*)me)->procedureId)

            FIELD("Visibility")
            procedure_visibility_t visibility = ((procedure_symbol_t*)me)->visibility;
            VALUE_STR(visibility == PUBLIC  ? "Public"  :
                      visibility == PRIVATE ? "Private" :
                      visibility == FRIEND  ? "Friend" : "Unknown");

            FIELD("IsVariadic")
            VALUE_NUM(((procedure_symbol_t*)me)->isVariadic)

            FIELD("Parameters")
            SUBSYMBOLS(((procedure_symbol_t*)me)->lsParameters, false)

            FIELD("ReturnType")
            SUBSYMBOL(((procedure_symbol_t*)me)->symReturnType, false)

            FIELD("Labels")
            SUBSYMBOLS(((procedure_symbol_t*)me)->lsLabels, false)

            FIELD_FINAL("Buckets")
            SUBSYMBOLS(((procedure_symbol_t*)me)->lsBuckets, true)

        END_SYMBOL()

        SYMBOL(PARAMETER_SYMBOL)

            FIELD("Name")
            VALUE_STR(me->name)

            FIELD("BucketIndex")
            VALUE_NUM(((parameter_symbol_t*)me)->bucketIndex)

            FIELD("PassingType")
            passing_type_t passingType = ((parameter_symbol_t*)me)->passingType;
            VALUE_STR(passingType == PASS_BY_REFERENCE  ? "ByRef"  :
                      passingType == PASS_BY_VALUE      ? "ByVal" : "Unknown");

            FIELD("IsOptional")
            VALUE_NUM(((parameter_symbol_t*)me)->isOptional)

            FIELD_FINAL("Type")
            SUBSYMBOL(((parameter_symbol_t*)me)->symType, true)

        END_SYMBOL()

        SYMBOL(LOCAL_VARIABLE_SYMBOL)

            FIELD("Name")
            VALUE_STR(me->name)

            FIELD("BucketIndex")
            VALUE_NUM(((local_variable_symbol_t*)me)->bucketIndex)

            FIELD_FINAL("Type")
            SUBSYMBOL(((local_variable_symbol_t*)me)->symType, true)

        END_SYMBOL()

        SYMBOL(LABEL_SYMBOL)

            FIELD("Name")
            VALUE_STR(me->name)

            FIELD_FINAL("LabelId")
            VALUE_NUM(((label_symbol_t*)me)->labelId);

        END_SYMBOL()

        default:
            printf("(unimplemented) \n");
    }
}


void DBG_PRETTY_PRINT_Print_SymbolList(symbol_list_t me) {
    for (int i = 0; i < me.length; ++i) {
        DBG_PRETTY_PRINT_Print_Symbol(me.symbols[i], i == me.length - 1);
    }
}