//
// Created by ds on 12/23/25.
//
#include "pretty_print.h"
#include "ansi.h"

#include <stdio.h>

#include "AST/Loose/ls_ast.h"
#include "AST/Loose/Clauses/arr_range_clause.h"
#include "AST/Loose/Clauses/dim_field_clause.h"
#include "AST/Loose/Clauses/parameter_clause.h"
#include "AST/Loose/Clauses/type_field_clause.h"
#include "AST/Loose/Expressions/literal_expression.h"
#include "AST/Loose/Expressions/reference_expression.h"
#include "AST/Loose/Members/function_member.h"
#include "AST/Loose/Members/module_member.h"
#include "AST/Loose/Members/sub_member.h"
#include "AST/Loose/Members/type_member.h"
#include "AST/Loose/Statements/assignment_statement.h"
#include "AST/Loose/Statements/dim_statement.h"
#include "AST/Loose/Statements/expression_statement.h"
#include "AST/Loose/Statements/redim_statement.h"

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

        default:;
    }
}

void DBG_PRETTY_PRINT_Print_LSAstNode_List(ls_ast_node_list_t me) {
    for (int i = 0; i < me.length; ++i) {
        DBG_PRETTY_PRINT_Print_LSAstNode(me.nodes[i], i == me.length - 1);
    }
}