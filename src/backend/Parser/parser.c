//
// Created by ds on 12/30/25.
//
#include <stdlib.h>
#include "parser.h"

#include <assert.h>

#include "AST/Loose/Clauses/arr_range_clause.h"
#include "AST/Loose/Clauses/type_field_clause.h"
#include "AST/Loose/Clauses/dim_field_clause.h"
#include "AST/Loose/Statements/dim_statement.h"
#include "AST/Loose/Statements/redim_statement.h"
#include "AST/Loose/Statements/assignment_statement.h"
#include "AST/Loose/Statements/goto_statement.h"
#include "AST/Loose/Statements/label_statement.h"
#include "AST/Loose/Statements/if_statement.h"
#include "AST/Loose/Statements/select_statement.h"
#include "AST/Loose/Statements/expression_statement.h"
#include "AST/Loose/Members/function_member.h"
#include "AST/Loose/Members/sub_member.h"
#include "Error/error.h"

token_t PARSER_EOL_Placeholder;

// =====================================================================================================================
// Public methods

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_Init:
// parser object constructor
parser_t *PARSER_Init(source_t source, token_list_t *tokens) {
    // allocate space for this instance
    parser_t *parser = malloc(sizeof(parser_t));

    // store our token list and source
    parser->tokens = tokens;
    parser->source = source;

    // initialize all other fields to zerp
    parser->pos = 0;
    parser->members.nodes = NULL;
    parser->members.length = 0;
    parser->members.capacity = 0;
    parser->hasError = false;
    parser->suppressError = 0;

    // also make sure our constant placeholder token is configured correctly
    PARSER_EOL_Placeholder.type = TK_EOF;

    return parser;
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_Unload:
// parser object destructor
void PARSER_Unload(parser_t *me) {
    // leave the member list and token list intact
    // only free this parser
    free(me);
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_Parse:
// Main parsing function
void PARSER_Parse(parser_t *me) {
    // while we're not at the end of file, or have hit an error -> parse members
    while (!AT_EOL()) {
        if (!PARSER_parseMember(me)) {
            break;
        }
    }
}

// =====================================================================================================================
// Private methods

// =====================================================================================================================
// MEMBERS
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseMember:
// parsing members like functions, subs, types or modules
bool PARSER_parseMember(parser_t *me) {
    switch (PS_CURRENT().type) {

        // Public / Private / Friend Functions or Subroutines
        case TK_KW_PUBLIC:
        case TK_KW_PRIVATE:
        case TK_KW_FRIEND:
        case TK_KW_STATIC:

            // if the next token is a function keyword -> parse this as a function
            if (PS_PEEK(1)->type == TK_KW_FUNCTION || (PS_PEEK(1)->type == TK_KW_STATIC && PS_PEEK(2)->type == TK_KW_FUNCTION)) {
                return PARSER_parseFunctionMember(me);
            }

            // if the next token is a sub keyword -> parse this as a subroutine
            if (PS_PEEK(1)->type == TK_KW_SUB || (PS_PEEK(1)->type == TK_KW_STATIC && PS_PEEK(2)->type == TK_KW_SUB)) {
                return PARSER_parseSubMember(me);
            }

            // everything else is illegal !!!!
            PS_ERROR_AT(SUB_PARSER, ERR_PS_UNEXPECTED_TOKEN, me->source, SPAN_FromToken(*PS_PEEK(1)), "Unexpected token, expected function or subroutine keywords")
            me->hasError = true;
            return false;

        // function keyword? -> this is totally a function
        case TK_KW_FUNCTION:
            return PARSER_parseFunctionMember(me);

        // subroutine keyword? -> this is toooootally a subroutine
        case TK_KW_SUB:
            return PARSER_parseSubMember(me);

        // may this be a custom user defined type?
        case TK_KW_TYPE:
            return PARSER_parseTypeMember(me);

        // module (lame af)
        case TK_KW_MODULE:
            return PARSER_parseModuleMember(me);

        // any EOS will be gobbled up, they dont matter
        case TK_EOS:
            PS_STEP() // skip over this
            return true;

        default:
            // found non member
            PS_ERROR_AT(SUB_PARSER, ERR_PS_UNEXPECTED_NON_MEMBER, me->source, SPAN_FromToken(PS_CURRENT()), "Global scope only allows for Module, Type, Function or Subroutine members")
            me->hasError = true;
            return false;
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseModuleMember:
// parse a module member :)
bool PARSER_parseModuleMember(parser_t *me) {

    // take in the 'Module' keyword
    token_t *kwModule = PARSER_consume(me, TK_KW_MODULE);
    RETURN_ON_ERROR()

    // take in the name of this module as an identifier
    token_t *idModule = PARSER_consume(me, TK_IDENTIFIER);
    RETURN_ON_ERROR()

    // we expect an end of statement after a module declaration
    PARSER_ffwToEOS(me);
    RETURN_ON_ERROR()

    // create the new node instance
    ls_module_member_node_t *node = malloc(sizeof(ls_module_member_node_t));
    node->base.type = LS_MODULE_MEMBER;
    node->kwModule     = kwModule;
    node->idModuleName = idModule;

    // append it to the global member list
    PS_LS_AST_NODE_LIST_Add(&me->members, (ls_ast_node_t*)node);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseTypeMember:
// parse a user defined type member :)
bool PARSER_parseTypeMember(parser_t *me) {

    // consume the 'Type' keyword
    token_t *kwType = PARSER_consume(me, TK_KW_TYPE);
    RETURN_ON_ERROR()

    // consume the name of this type as an identifier
    token_t *idTypeName = PARSER_consume(me, TK_IDENTIFIER);
    RETURN_ON_ERROR()

    // expect an end of line after
    PARSER_ffwToEOS(me);
    RETURN_ON_ERROR()

    // initialize the list for our field nodes
    ls_ast_node_list_t lsFields = PS_LS_AST_NODE_LIST_Init();

    while (PS_CURRENT().type != TK_KW_END) {

        // consume a field name identifier
        token_t *idFieldName = PARSER_consume(me, TK_IDENTIFIER);
        BREAK_ON_ERROR()

        // consume the type as an 'As' clause
        ls_as_clause_node_t *clsFieldType = PARSER_parseAsClause(me, false, false);
        BREAK_ON_ERROR(
            UNLOAD_IF_NOT_NULL(clsFieldType)
        )

        // expect an EOS
        PARSER_ffwToEOS(me);
        BREAK_ON_ERROR(
            UNLOAD_IF_NOT_NULL(clsFieldType)
        )

        // allocate a new node for this type field
        ls_type_field_clause_node_t *clsTypeField = malloc(sizeof(ls_type_field_clause_node_t));
        clsTypeField->base.type   = LS_TYPE_FIELD_CLAUSE;
        clsTypeField->idFieldName = idFieldName;
        clsTypeField->clsType     = clsFieldType;

        // add it to the list
        PS_LS_AST_NODE_LIST_Add(&lsFields, (ls_ast_node_t*)clsTypeField);
    }

    // get the hell out of here if everything went to shit
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsFields);
    )


    // consume the 'End' keyword
    token_t *kwEnd = PARSER_consume(me, TK_KW_END);
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsFields);
    )

    // consume the ending 'Type' keyword
    token_t *kwEndType = PARSER_consume(me, TK_KW_TYPE);
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsFields);
    )

    // end with one of the many classic line endings like \n
    PARSER_ffwToEOS(me);
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsFields);
    )

    // if nothing died on our way here -> allocate a node for this :)
    ls_type_member_node_t *node = malloc(sizeof(ls_type_member_node_t));
    node->base.type  = LS_TYPE_MEMBER;
    node->kwType     = kwType;
    node->idTypeName = idTypeName;
    node->lsFields   = lsFields;
    node->kwEnd      = kwEnd;
    node->kwEndType  = kwEndType;

    // and add it to the global member list
    PS_LS_AST_NODE_LIST_Add(&me->members, (ls_ast_node_t*)node);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseFunctionMember:
// parse a user defined function member
bool PARSER_parseFunctionMember(parser_t *me) {
    // initialize these as null, at most one of these is allowed to be set
    token_t *kwPublic  = NULL;
    token_t *kwPrivate = NULL;
    token_t *kwFriend  = NULL;
    token_t *kwStatic  = NULL;

    // if theres a 'Public' keyword, consume it
    if (PS_CURRENT().type == TK_KW_PUBLIC) {
        kwPublic = PARSER_consume(me, TK_KW_PUBLIC);
    }

    // if theres a 'Private' keyword, consume it
    if (PS_CURRENT().type == TK_KW_PRIVATE) {
        kwPrivate = PARSER_consume(me, TK_KW_PRIVATE);
    }

    // if theres a 'Friend' keyword, consume it
    if (PS_CURRENT().type == TK_KW_FRIEND) {
        kwFriend = PARSER_consume(me, TK_KW_FRIEND);
    }

    // if theres a 'Static' keyword, consume it
    if (PS_CURRENT().type == TK_KW_STATIC) {
        kwStatic = PARSER_consume(me, TK_KW_STATIC);
    }


    // consume the 'Function' keyword
    token_t *kwFunction = PARSER_consume(me, TK_KW_FUNCTION);
    RETURN_ON_ERROR()

    // consume the function name as an identifier
    token_t *idName = PARSER_consume(me, TK_IDENTIFIER);
    RETURN_ON_ERROR()

    // consume the opening parenthesis for the parameter list
    token_t *pcOpenParenthesis = PARSER_consume(me, TK_PC_OPEN_PARENTHESIS);
    RETURN_ON_ERROR()

    // set up a list for our parameters
    ls_ast_node_list_t lsParameters = PS_LS_AST_NODE_LIST_Init();

    // take in parameters for as long as we dont find a closing parenthesis
    while (PS_CURRENT().type != TK_PC_CLOSED_PARENTHESIS) {

        // parse a parameter
        ls_parameter_clause_node_t *param = PARSER_parseParameterClause(me);

        // break out if anything went wrong
        if (param == NULL || me->hasError) {
            break;
        }

        PS_LS_AST_NODE_LIST_Add(&lsParameters, (ls_ast_node_t*)param);

        // if we're not at the closing parenthesis, expect a comma
        if (PS_CURRENT().type != TK_PC_CLOSED_PARENTHESIS) {
            PARSER_consume(me, TK_PC_COMMA);
            BREAK_ON_ERROR()
        }
    }
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsParameters);
    )

    // consume the closing parenthesis
    token_t *pcClosedParenthesis = PARSER_consume(me, TK_PC_CLOSED_PARENTHESIS);
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsParameters);
    )

    // was there a return type given?
    ls_as_clause_node_t *clsReturnType = NULL;
    if (PS_CURRENT().type == TK_KW_AS) {
        clsReturnType = PARSER_parseAsClause(me, true, false);
    }
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsParameters);
        UNLOAD_IF_NOT_NULL(clsReturnType)
    )

    // expect an EOS after the signature line
    PARSER_ffwToEOS(me);
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsParameters);
        UNLOAD_IF_NOT_NULL(clsReturnType)
    )

    // create a new node list for the function body
    ls_ast_node_list_t lsBody = PS_LS_AST_NODE_LIST_Init();

    // parse the function body until we reach an 'End' keyword
    token_type_t terminator = TK_KW_END;
    PARSER_parseBlockOfStatements(me, &lsBody, &terminator, 1);
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsParameters);
        PS_LS_AST_NODE_LIST_Unload(lsBody);
        UNLOAD_IF_NOT_NULL(clsReturnType)
    )

    // consume the 'End' keyword
    token_t *kwEnd = PARSER_consume(me, TK_KW_END);
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsParameters);
        PS_LS_AST_NODE_LIST_Unload(lsBody);
        UNLOAD_IF_NOT_NULL(clsReturnType)
    )

    // consume the ending 'Function' keyword
    token_t *kwEndFunction = PARSER_consume(me, TK_KW_FUNCTION);
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsParameters);
        PS_LS_AST_NODE_LIST_Unload(lsBody);
        UNLOAD_IF_NOT_NULL(clsReturnType)
    )

    // yet again, end with one of the many classic line endings like \n
    PARSER_ffwToEOS(me);
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsParameters);
        PS_LS_AST_NODE_LIST_Unload(lsBody);
        UNLOAD_IF_NOT_NULL(clsReturnType)
    )

    // if we didnt crash and burn on our way here -> create a member node
    ls_function_member_node_t *node = malloc(sizeof(ls_function_member_node_t));
    node->base.type = LS_FUNCTION_MEMBER;
    node->kwPublic            = kwPublic;
    node->kwPrivate           = kwPrivate;
    node->kwFriend            = kwFriend;
    node->kwStatic            = kwStatic;
    node->kwFunction          = kwFunction;
    node->idName              = idName;
    node->pcOpenParenthesis   = pcOpenParenthesis;
    node->lsParameters        = lsParameters;
    node->pcClosedParenthesis = pcClosedParenthesis;
    node->clsReturnType       = clsReturnType;
    node->lsFunctionBody      = lsBody;
    node->kwEnd               = kwEnd;
    node->kwEndFunction       = kwEndFunction;

    // add it to the global member list
    PS_LS_AST_NODE_LIST_Add(&me->members, (ls_ast_node_t*)node);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseSubMember:
// parse a user defined subroutine member, this is largely the same as the function member one
bool PARSER_parseSubMember(parser_t *me) {
    // initialize these as null, at most one of these is allowed to be set
    token_t *kwPublic  = NULL;
    token_t *kwPrivate = NULL;
    token_t *kwFriend  = NULL;
    token_t *kwStatic  = NULL;

    // if theres a 'Public' keyword, consume it
    if (PS_CURRENT().type == TK_KW_PUBLIC) {
        kwPublic = PARSER_consume(me, TK_KW_PUBLIC);
    }

    // if theres a 'Private' keyword, consume it
    if (PS_CURRENT().type == TK_KW_PRIVATE) {
        kwPrivate = PARSER_consume(me, TK_KW_PRIVATE);
    }

    // if theres a 'Friend' keyword, consume it
    if (PS_CURRENT().type == TK_KW_FRIEND) {
        kwFriend = PARSER_consume(me, TK_KW_FRIEND);
    }

    // if theres a 'Static' keyword, consume it
    if (PS_CURRENT().type == TK_KW_STATIC) {
        kwStatic = PARSER_consume(me, TK_KW_STATIC);
    }


    // consume the 'Sub' keyword
    token_t *kwSub = PARSER_consume(me, TK_KW_SUB);
    RETURN_ON_ERROR()

    // consume the function name as an identifier
    token_t *idName = PARSER_consume(me, TK_IDENTIFIER);
    RETURN_ON_ERROR()

    // consume the opening parenthesis for the parameter list
    token_t *pcOpenParenthesis = PARSER_consume(me, TK_PC_OPEN_PARENTHESIS);
    RETURN_ON_ERROR()

    // set up a list for our parameters
    ls_ast_node_list_t lsParameters = PS_LS_AST_NODE_LIST_Init();

    // take in parameters for as long as we dont find a closing parenthesis
    while (PS_CURRENT().type != TK_PC_CLOSED_PARENTHESIS) {

        // parse a parameter
        ls_parameter_clause_node_t *param = PARSER_parseParameterClause(me);

        // break out if anything went wrong
        if (param == NULL || me->hasError) {
            break;
        }

        PS_LS_AST_NODE_LIST_Add(&lsParameters, (ls_ast_node_t*)param);

        // if we're not at the closing parenthesis, expect a comma
        if (PS_CURRENT().type != TK_PC_CLOSED_PARENTHESIS) {
            PARSER_consume(me, TK_PC_COMMA);
            BREAK_ON_ERROR()
        }
    }
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsParameters);
    )

    // consume the closing parenthesis
    token_t *pcClosedParenthesis = PARSER_consume(me, TK_PC_CLOSED_PARENTHESIS);
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsParameters);
    )

    // expect an EOS after the signature line
    PARSER_ffwToEOS(me);
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsParameters);
    )

    // create a new node list for the function body
    ls_ast_node_list_t lsBody = PS_LS_AST_NODE_LIST_Init();

    // parse the function body until we reach an 'End' keyword
    token_type_t terminator = TK_KW_END;
    PARSER_parseBlockOfStatements(me, &lsBody, &terminator, 1);
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsParameters);
        PS_LS_AST_NODE_LIST_Unload(lsBody);
    )

    // consume the 'End' keyword
    token_t *kwEnd = PARSER_consume(me, TK_KW_END);
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsParameters);
        PS_LS_AST_NODE_LIST_Unload(lsBody);
    )

    // consume the ending 'Sub' keyword
    token_t *kwEndSub= PARSER_consume(me, TK_KW_SUB);
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsParameters);
        PS_LS_AST_NODE_LIST_Unload(lsBody);
    )

    // yet again, end with one of the many classic line endings like \n
    PARSER_ffwToEOS(me);
    RETURN_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsParameters);
        PS_LS_AST_NODE_LIST_Unload(lsBody);
    )

    // if we didnt crash and burn on our way here -> create a member node
    ls_sub_member_node_t *node = malloc(sizeof(ls_sub_member_node_t));
    node->base.type = LS_SUBROUTINE_MEMBER;
    node->kwPublic            = kwPublic;
    node->kwPrivate           = kwPrivate;
    node->kwFriend            = kwFriend;
    node->kwStatic            = kwStatic;
    node->kwSub               = kwSub;
    node->idName              = idName;
    node->pcOpenParenthesis   = pcOpenParenthesis;
    node->lsParameters        = lsParameters;
    node->pcClosedParenthesis = pcClosedParenthesis;
    node->lsSubBody           = lsBody;
    node->kwEnd               = kwEnd;
    node->kwEndSub            = kwEndSub;

    // add it to the global member list
    PS_LS_AST_NODE_LIST_Add(&me->members, (ls_ast_node_t*)node);
    return true;
}

// =====================================================================================================================
// CLAUSES
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseAsClause:
// parse an as clause, used for setting the type of a variable, field, or function
// supports function notation and non function notation
ls_as_clause_node_t *PARSER_parseAsClause(parser_t *me, bool functionNotation, bool allowArrayRanges) {
    token_t *pcOpenParenthesis   = NULL;
    token_t *pcClosedParenthesis = NULL;

    ls_ast_node_list_t lsArrayRanges = PS_LS_AST_NODE_LIST_Init();

    // when this is not sub or function notation the array parentheses follow the identifier
    // Dim XXXX() As Integer
    //         ^^
    if (!functionNotation && PS_CURRENT().type == TK_PC_OPEN_PARENTHESIS) {
        pcOpenParenthesis   = PARSER_consume(me, TK_PC_OPEN_PARENTHESIS);

        // when array ranges are allowed, parse as many as we can
        // ] (10 To 100, 10, 20 To 30, ...)
        if (allowArrayRanges) {
            while (PS_CURRENT().type != TK_PC_CLOSED_PARENTHESIS) {

                // read in a first number literal
                ls_ast_node_t *bound = PARSER_parseExpression(me);
                RETURN_NULL_ON_ERROR(
                    PS_LS_AST_NODE_LIST_Unload(lsArrayRanges);
                    UNLOAD_IF_NOT_NULL(bound)
                )

                // when theres a 'To' keyword -> there are two bounds
                if (PS_CURRENT().type == TK_KW_TO) {

                    // read in the 'To' keyword
                    token_t *kwTo = PARSER_consume(me, TK_KW_TO);

                    // also read in the ending literal
                    ls_ast_node_t *ubound = PARSER_parseExpression(me);

                    // bail when shit goes wrong
                    RETURN_NULL_ON_ERROR(
                        PS_LS_AST_NODE_LIST_Unload(lsArrayRanges);
                        UNLOAD_IF_NOT_NULL(bound)
                        UNLOAD_IF_NOT_NULL(ubound)
                    )

                    // if we got the full
                    // <Number> To <Number>
                    // sequence, add a full lower and upper bound array range node
                    ls_arr_range_clause_node_t *rangeNode = malloc(sizeof(ls_arr_range_clause_node_t));
                    rangeNode->base.type = LS_ARR_RANGE_CLAUSE;
                    rangeNode->ltLBound = bound;
                    rangeNode->kwTo = kwTo;
                    rangeNode->ltUBound = ubound;

                    // add it to the range list
                    PS_LS_AST_NODE_LIST_Add(&lsArrayRanges, (ls_ast_node_t*)rangeNode);
                }
                else {
                    // otherwise, we only have an upper bound
                    // <Number>
                    // as in: Dim aMyArray(10)
                    ls_arr_range_clause_node_t *rangeNode = malloc(sizeof(ls_arr_range_clause_node_t));
                    rangeNode->base.type = LS_ARR_RANGE_CLAUSE;
                    rangeNode->ltLBound = NULL;
                    rangeNode->kwTo     = NULL;
                    rangeNode->ltUBound = bound;

                    // add it to the range list
                    PS_LS_AST_NODE_LIST_Add(&lsArrayRanges, (ls_ast_node_t*)rangeNode);
                }

                // consume a comma if we're not at the end of the list yet
                if (PS_CURRENT().type != TK_PC_CLOSED_PARENTHESIS) {
                    // commation
                    PARSER_consume(me, TK_PC_COMMA);
                    RETURN_NULL_ON_ERROR(
                        PS_LS_AST_NODE_LIST_Unload(lsArrayRanges);
                    )
                }
            }
        }

        pcClosedParenthesis = PARSER_consume(me, TK_PC_CLOSED_PARENTHESIS);

        // return if we didnt get what we expected
        RETURN_NULL_ON_ERROR(
            PS_LS_AST_NODE_LIST_Unload(lsArrayRanges);
        )

        // if the next token is not an 'As' keyword, then this is a shortend, implicit as clause
        if (PS_CURRENT().type != TK_KW_AS) {

            // -> return a clause with everything being empty except for the array marks
            ls_as_clause_node_t *node = malloc(sizeof(ls_as_clause_node_t));
            node->base.type = LS_AS_CLAUSE;
            node->kwAs   = NULL;
            node->idType = NULL;
            node->pcOpenParenthesis   = pcOpenParenthesis;
            node->pcClosedParenthesis = pcClosedParenthesis;
            node->lsArrRanges         = lsArrayRanges;

            return node;
        }

    }

    // consume the 'As' keyword
    token_t *kwAs = PARSER_consume(me, TK_KW_AS);
    RETURN_NULL_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsArrayRanges);
    )

    // consume the type identifier
    token_t *idTypeName = PARSER_consume(me, TK_IDENTIFIER);
    RETURN_NULL_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsArrayRanges);
    )

    // when this is sub or function notation the array parentheses follow the type name
    // Function Test() As Integer()
    //                           ^^
    if (functionNotation && PS_CURRENT().type == TK_PC_OPEN_PARENTHESIS) {
        pcOpenParenthesis   = PARSER_consume(me, TK_PC_OPEN_PARENTHESIS);
        pcClosedParenthesis = PARSER_consume(me, TK_PC_CLOSED_PARENTHESIS);

        // same as above :)
        RETURN_NULL_ON_ERROR(
            PS_LS_AST_NODE_LIST_Unload(lsArrayRanges);
        )
    }

    // allocate a new clause node and bring over its values
    ls_as_clause_node_t *node = malloc(sizeof(ls_as_clause_node_t));
    node->base.type = LS_AS_CLAUSE;
    node->kwAs   = kwAs;
    node->idType = idTypeName;
    node->pcOpenParenthesis   = pcOpenParenthesis;
    node->pcClosedParenthesis = pcClosedParenthesis;
    node->lsArrRanges         = lsArrayRanges;

    return node;
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseParameterClause:
// parses a function of subroutine parameter
ls_parameter_clause_node_t *PARSER_parseParameterClause(parser_t *me) {
    // initialize all optional modifiers as null
    token_t *kwOptional = NULL;
    token_t *kwByVal = NULL;
    token_t *kwByRef = NULL;
    token_t *kwParamArray = NULL;

    // if theres an 'Optional' keyword, consume it
    if (PS_CURRENT().type == TK_KW_OPTIONAL) {
        kwOptional = PARSER_consume(me, TK_KW_OPTIONAL);
    }

    // if theres a 'ByVal' keyword, consume it
    if (PS_CURRENT().type == TK_KW_BYVAL) {
        kwByVal = PARSER_consume(me, TK_KW_BYVAL);
    }

    // if theres a 'ByRef' keyword, consume it
    if (PS_CURRENT().type == TK_KW_BYREF) {
        kwByRef = PARSER_consume(me, TK_KW_BYREF);
    }

    // if theres a 'ParamArray' keyword, consume it
    if (PS_CURRENT().type == TK_KW_PARAMARRAY) {
        kwParamArray = PARSER_consume(me, TK_KW_PARAMARRAY);
    }

    // consume the parameter name
    token_t *idName = PARSER_consume(me, TK_IDENTIFIER);
    RETURN_NULL_ON_ERROR()

    // if there is a type, consume it
    ls_as_clause_node_t *clsType = NULL;
    if (PS_CURRENT().type == TK_PC_OPEN_PARENTHESIS ||
        PS_CURRENT().type == TK_KW_AS) {
        clsType = PARSER_parseAsClause(me, false, false);
    }
    RETURN_NULL_ON_ERROR(
        UNLOAD_IF_NOT_NULL(clsType);
    )

    // create placeholders for a potential default value
    token_t *opEquals = NULL;
    ls_ast_node_t *expDefaultValue = NULL;

    // if theres a default value, consume it as well
    if (PS_CURRENT().type == TK_OP_EQUALS) {
        opEquals = PARSER_consume(me, TK_OP_EQUALS);
        expDefaultValue = PARSER_parseExpression(me);
    }
    RETURN_NULL_ON_ERROR(
        UNLOAD_IF_NOT_NULL(clsType);
        UNLOAD_IF_NOT_NULL(expDefaultValue);
    )

    // if everything worked out we should be ready to return a parameter node
    ls_parameter_clause_node_t *node = malloc(sizeof(ls_parameter_clause_node_t));
    node->base.type = LS_PARAMETER_CLAUSE;
    node->kwOptional      = kwOptional;
    node->kwByVal         = kwByVal;
    node->kwByRef         = kwByRef;
    node->kwParamArray    = kwParamArray;
    node->idParamName     = idName;
    node->clsType         = clsType;
    node->opEquals        = opEquals;
    node->expDefaultValue = expDefaultValue;

    return node;
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseConditionalClause:
// parse conditional clause like an if or an elseif block
ls_conditional_clause_node_t *PARSER_parseConditionalClause(parser_t *me, token_type_t expectedConditionalKeyword) {

    // parse the expected conditional keyword for this clause
    // based on the context it may be If, ElseIf, or Case
    token_t *kwConditional = PARSER_consume(me, expectedConditionalKeyword);
    RETURN_NULL_ON_ERROR()

    // parse the condition
    ls_ast_node_t *exprCondition = PARSER_parseExpression(me);
    RETURN_NULL_ON_ERROR(
        UNLOAD_IF_NOT_NULL(exprCondition);
    )

    // if this is not a case statement, expect a Then keyword
    token_t *kwThen = NULL;
    if (expectedConditionalKeyword != TK_KW_CASE) {
        kwThen = PARSER_consume(me, TK_KW_THEN);
    }
    RETURN_NULL_ON_ERROR(
        UNLOAD_IF_NOT_NULL(exprCondition);
    )

    // expect an EOS
    PARSER_ffwToEOS(me);
    RETURN_NULL_ON_ERROR(
        UNLOAD_IF_NOT_NULL(exprCondition);
    )

    // parse a block of statements until we either hit an ElseIf, Else, Case, or End keyword
    token_type_t terminators[4] = {
        TK_KW_ELSEIF,
        TK_KW_ELSE,
        TK_KW_CASE,
        TK_KW_END,
    };
    ls_ast_node_list_t lsStatements = PS_LS_AST_NODE_LIST_Init();
    PARSER_parseBlockOfStatements(me, &lsStatements, terminators, 4);
    RETURN_NULL_ON_ERROR(
        UNLOAD_IF_NOT_NULL(exprCondition);
        PS_LS_AST_NODE_LIST_Unload(lsStatements);
    )

    // if everything went alright, allocate a new node and return it
    ls_conditional_clause_node_t *node = malloc(sizeof(ls_conditional_clause_node_t));
    node->base.type = LS_CONDITIONAL_CLAUSE;
    node->kwConditional = kwConditional;
    node->exprCondition = exprCondition;
    node->kwThen = kwThen;
    node->lsStatements = lsStatements;
    return node;
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseElseClause:
// parse the else block of an if statement
ls_else_clause_node_t *PARSER_parseElseClause(parser_t *me) {

    // if theres a Case keyword, consume it
    token_t *kwCase = NULL;
    if (PS_CURRENT().type == TK_KW_CASE) {
        kwCase = PARSER_consume(me, TK_KW_CASE);
    }

    // expect an Else keyword
    token_t *kwElse = PARSER_consume(me, TK_KW_ELSE);
    RETURN_NULL_ON_ERROR()

    // expect an EOS
    PARSER_ffwToEOS(me);
    RETURN_NULL_ON_ERROR()

    // parse a block of statements until we hit an End keyword
    token_type_t terminator = TK_KW_END;
    ls_ast_node_list_t lsStatements = PS_LS_AST_NODE_LIST_Init();
    PARSER_parseBlockOfStatements(me, &lsStatements, &terminator, 1);
    RETURN_NULL_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsStatements);
    )

    // if we did it, allocate a new node and return it
    ls_else_clause_node_t *node = malloc(sizeof(ls_else_clause_node_t));
    node->base.type = LS_ELSE_CLAUSE;
    node->kwElse = kwElse;
    node->lsStatements = lsStatements;
    return node;
}

// =====================================================================================================================
// STATEMENTS
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseBlockOfStatements:
// parse a list or 'block' of statements until a terminator token is reached, abort on error
void PARSER_parseBlockOfStatements(parser_t *me, ls_ast_node_list_t *lsBody, token_type_t *until, uint8_t untilCount) {

    // parse statements for as long as we havent reached our terminator character
    while (true) {

        // have we encountered one of the terminators
        for (int i = 0; i < untilCount; ++i) {
            if (PS_CURRENT().type == until[i]) return;
        }

        // skip any EOS-es in front
        while (PS_CURRENT().type == TK_EOS) { PS_STEP() }

        // check again if we have encountered one of the terminators
        for (int i = 0; i < untilCount; ++i) {
            if (PS_CURRENT().type == until[i]) return;
        }

        // parse this statement
        ls_ast_node_t *stmt = PARSER_parseStatement(me);

        // is there any sign of something going wrong?
        if (stmt == NULL || me->hasError) {
            // if so, break, this will be cascaded down
            break;
        }

        // if everything is fine we add it to the list and go to the next statement
        PS_LS_AST_NODE_LIST_Add(lsBody, stmt);

        // skip any EOS-es in the back
        //while (PS_CURRENT().type == TK_EOS) { PS_STEP() }
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseStatement:
// parse a generic statement, any statement we come across
ls_ast_node_t *PARSER_parseStatement(parser_t *me) {
    ls_ast_node_t *stmt = NULL;

    // STATEMENTS :O
    switch (PS_CURRENT().type) {

        case TK_KW_DIM:
            stmt = PARSER_parseDimStatement(me);
        break;

        case TK_KW_REDIM:
            stmt = PARSER_parseReDimStatement(me);
        break;

        case TK_KW_LET:
        case TK_KW_SET:
            stmt = PARSER_parseAssignmentStatement(me, NULL);
        break;

        case TK_KW_IF:
            stmt = PARSER_parseIfStatement(me);
        break;

        case TK_KW_SELECT:
            stmt = PARSER_parseSelectStatement(me);
        break;

        case TK_KW_GOTO:
            stmt = PARSER_parseGotoStatement(me);
        break;

        case TK_IDENTIFIER:
            if (PS_PEEK(1)->type == TK_EOS && PS_PEEK(1)->strValue != NULL) {
                stmt = PARSER_parseLabelStatement(me);
                break;
            }

        default:
            // if we didnt get any of these, try parsing an expression statement
            SNAPSHOT(TRY_EXPRESSION_STATEMENT)
            stmt = PARSER_parseExpressionStatement(me);
            ROLLBACK(TRY_EXPRESSION_STATEMENT)

            // did it work?
            if (!me->hasError) {
                // parse it again because we rolled back the parser
                // not great performance wise but theres nothing else i can do with the current error system
                // this will have to be refactored in the future
                UNLOAD_IF_NOT_NULL(stmt);
                stmt = PARSER_parseExpressionStatement(me);
            }

            // otherwise, throw an error
            else {
                me->hasError = true;
                ERROR_AT(SUB_PARSER, ERR_PS_UNEXPECTED_NON_STATEMENT, me->source, SPAN_FromToken(PS_CURRENT()), "Expected a Statement")
            }
    }
    RETURN_NULL_ON_ERROR(
        UNLOAD_IF_NOT_NULL(stmt)
    )

    // if the expression we parsed was a reference expression and we've encountered an equals
    // -> this is actually an implicit assigment expression
    if (stmt->type == LS_EXPRESSION_STATEMENT) {
        if (PS_CURRENT().type == TK_OP_EQUALS
            && ((ls_expression_statement_node_t*)stmt)->exprExpression->type == LS_REFERENCE_EXPRESSION) {

            // parse this as an assignment statement
            ls_ast_node_t *newStmt = PARSER_parseAssignmentStatement(me, ((ls_expression_statement_node_t*)stmt)->exprExpression);
            RETURN_NULL_ON_ERROR(
                UNLOAD_IF_NOT_NULL(stmt)
                UNLOAD_IF_NOT_NULL(newStmt)
            )

            // free the old statement node
            free(stmt);

            // replace it with a new statement
            stmt = newStmt;
        }
    }

    // totally require an EOS after every statement
    PARSER_ffwToEOS(me);
    return stmt;
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseDimStatement:
// literally parse a 'Dim' statement, these comments are crazy helpful
ls_ast_node_t *PARSER_parseDimStatement(parser_t *me) {
    // consume the 'Dim' keyword
    token_t *kwDim = PARSER_consume(me, TK_KW_DIM);
    RETURN_NULL_ON_ERROR()

    // initialize a list for all our dimmed variables
    ls_ast_node_list_t lsDimFields = PS_LS_AST_NODE_LIST_Init();

    // until we hit the EOS, consume fields
    while (PS_CURRENT().type != TK_EOS) {

        // consume the identifier
        token_t *idName = PARSER_consume(me, TK_IDENTIFIER);
        BREAK_ON_ERROR()

        // create an empty as clause for the type of this field
        ls_as_clause_node_t *clsType = NULL;

        // if theres a type clause, consume it
        if (PS_CURRENT().type == TK_PC_OPEN_PARENTHESIS || PS_CURRENT().type == TK_KW_AS) {
            clsType = PARSER_parseAsClause(me, false, true);
        }
        BREAK_ON_ERROR(
            UNLOAD_IF_NOT_NULL(clsType)
        )

        // create a node for this field
        ls_dim_field_clause_node_t *clsDimField = malloc(sizeof(ls_dim_field_clause_node_t));
        clsDimField->base.type = LS_DIM_FIELD_CLAUSE;
        clsDimField->idName  = idName;
        clsDimField->clsType = clsType;

        // add it to the list
        PS_LS_AST_NODE_LIST_Add(&lsDimFields, (ls_ast_node_t*)clsDimField);

        // if we're not at the EOS yet, expect a comma
        if (PS_CURRENT().type != TK_EOS) {
            PARSER_consume(me, TK_PC_COMMA);
            BREAK_ON_ERROR()
        }
    }
    RETURN_NULL_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsDimFields);
    )

    // create a new node for this statement
    ls_dim_statement_node_t *node = malloc(sizeof(ls_dim_statement_node_t));
    node->base.type = LS_DIM_STATEMENT;
    node->kwDim       = kwDim;
    node->lsDimFields = lsDimFields;

    return (ls_ast_node_t*)node;
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseReDimStatement:
// parse a 'ReDim' statement
ls_ast_node_t *PARSER_parseReDimStatement(parser_t *me) {
    // consume the 'ReDim' keyword
    token_t *kwReDim = PARSER_consume(me, TK_KW_REDIM);
    RETURN_NULL_ON_ERROR()

    // consume a 'Preserve' keyword if there is one
    token_t *kwPreserve = NULL;
    if (PS_CURRENT().type == TK_KW_PRESERVE) {
        kwPreserve = PARSER_consume(me, TK_KW_PRESERVE);
    }

    // initialize a list for all our dimmed variables
    ls_ast_node_list_t lsDimFields = PS_LS_AST_NODE_LIST_Init();

    // until we hit the EOS, consume fields
    while (PS_CURRENT().type != TK_EOS) {

        // consume the identifier
        token_t *idName = PARSER_consume(me, TK_IDENTIFIER);
        BREAK_ON_ERROR()

        // make sure to enforce an open parenthesis here
        PARSER_consume(me, TK_PC_OPEN_PARENTHESIS);
        BREAK_ON_ERROR()
        me->pos--;

        // consume an 'As' clause for the redim
        ls_as_clause_node_t *clsType = PARSER_parseAsClause(me, false, true);
        BREAK_ON_ERROR(
            UNLOAD_IF_NOT_NULL(clsType)
        )

        // because the 'As' clause is fairly liberal, make sure its valid for this purpose

        // make sure this is an array type 'as' clause
        if (clsType->pcOpenParenthesis == NULL) {
            PS_ERROR_AT(SUB_PARSER, ERR_PS_UNEXPECTED_AS_CLAUSE, me->source, SPAN_FromToken(*idName), "ReDim expects an 'As' clause of type array")
            me->hasError = true;
        }
        BREAK_ON_ERROR(
            UNLOAD_IF_NOT_NULL(clsType)
        )

        // make sure it has at least one range
        if (clsType->lsArrRanges.length == 0) {
            PS_ERROR_AT(SUB_PARSER, ERR_PS_UNEXPECTED_AS_CLAUSE, me->source, SPAN_FromToken(*clsType->pcOpenParenthesis), "ReDim expects an array type of a specified size")
            me->hasError = true;
        }
        BREAK_ON_ERROR(
            UNLOAD_IF_NOT_NULL(clsType)
        )

        // make sure theres no retyping going on
        if (clsType->kwAs != NULL) {
            PS_ERROR_AT(SUB_PARSER, ERR_PS_UNEXPECTED_AS_CLAUSE, me->source, SPAN_FromToken(*clsType->kwAs), "ReDim does not allow for retyping")
            me->hasError = true;
        }
        BREAK_ON_ERROR(
            UNLOAD_IF_NOT_NULL(clsType)
        )


        // create a node for this field
        ls_dim_field_clause_node_t *clsDimField = malloc(sizeof(ls_dim_field_clause_node_t));
        clsDimField->base.type = LS_DIM_FIELD_CLAUSE;
        clsDimField->idName  = idName;
        clsDimField->clsType = clsType;

        // add it to the list
        PS_LS_AST_NODE_LIST_Add(&lsDimFields, (ls_ast_node_t*)clsDimField);

        // if we're not at the EOS yet, expect a comma
        if (PS_CURRENT().type != TK_EOS) {
            PARSER_consume(me, TK_PC_COMMA);
            BREAK_ON_ERROR()
        }
    }
    RETURN_NULL_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsDimFields);
    )

    // create a new node for this statement
    ls_redim_statement_node_t *node = malloc(sizeof(ls_redim_statement_node_t));
    node->base.type = LS_REDIM_STATEMENT;
    node->kwReDim     = kwReDim;
    node->kwPreserve  = kwPreserve;
    node->lsDimFields = lsDimFields;

    return (ls_ast_node_t*)node;
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseGotoStatement:
// parse a classic good old goto statement :)
ls_ast_node_t *PARSER_parseGotoStatement(parser_t *me) {

    // consume the goto keyword
    token_t *kwGoto = PARSER_consume(me, TK_KW_GOTO);
    RETURN_NULL_ON_ERROR()

    // consume the label we're jumping to
    token_t *idLabel = PARSER_consume(me, TK_IDENTIFIER);
    RETURN_NULL_ON_ERROR()

    // allocate and return a new node
    ls_goto_statement_node_t *node = malloc(sizeof(ls_goto_statement_node_t));
    node->base.type = LS_GOTO_STATEMENT;
    node->kwGoto    = kwGoto;
    node->idLabel   = idLabel;
    return (ls_ast_node_t*)node;
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseLabelStatement:
// parse a lil label
ls_ast_node_t *PARSER_parseLabelStatement(parser_t *me) {

    // parse the labels identfier
    token_t *idLabel = PARSER_consume(me, TK_IDENTIFIER);
    RETURN_NULL_ON_ERROR()

    // allocate and return this node
    ls_label_statement_node_t *node = malloc(sizeof(ls_label_statement_node_t));
    node->base.type = LS_LABEL_STATEMENT;
    node->idLabel   = idLabel;
    return (ls_ast_node_t*)node;
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseIfStatement:
// parse an if statement with all its elseifs and an else block
ls_ast_node_t *PARSER_parseIfStatement(parser_t *me) {

    // expect this statement to begin with an if
    PARSER_consume(me, TK_KW_IF);
    RETURN_NULL_ON_ERROR()

    // roll this back, because the keyword will actually be parsed by a conditional clause
    PS_STEP_BACK();

    // create a list for our conditionals
    ls_ast_node_list_t lsConditionals = PS_LS_AST_NODE_LIST_Init();

    // parse the If block conditional
    ls_conditional_clause_node_t *clsIf = PARSER_parseConditionalClause(me, TK_KW_IF);
    RETURN_NULL_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsConditionals);
    )

    // add it as the first conditional in our list
    PS_LS_AST_NODE_LIST_Add(&lsConditionals, (ls_ast_node_t*)clsIf);

    // parse ElseIf conditionals for as long as we encounter ElseIf keywords
    while (PS_CURRENT().type == TK_KW_ELSEIF) {

        // parse the ElseIf clause
        ls_conditional_clause_node_t *clsElseIf = PARSER_parseConditionalClause(me, TK_KW_ELSEIF);
        RETURN_NULL_ON_ERROR(
            UNLOAD_IF_NOT_NULL(clsElseIf)
            PS_LS_AST_NODE_LIST_Unload(lsConditionals);
        )

        // add it to the list
        PS_LS_AST_NODE_LIST_Add(&lsConditionals, (ls_ast_node_t*)clsElseIf);
    }


    // if theres an Else clause, parse it as well
    ls_else_clause_node_t *clsElse = NULL;
    if (PS_CURRENT().type == TK_KW_ELSE) {
        clsElse = PARSER_parseElseClause(me);
    }
    RETURN_NULL_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsConditionals);
        UNLOAD_IF_NOT_NULL(clsElse);
    )

    // consume the End If marker
    token_t *kwEnd = PARSER_consume(me, TK_KW_END);
    RETURN_NULL_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsConditionals);
        UNLOAD_IF_NOT_NULL(clsElse);
    )

    token_t *kwEndIf = PARSER_consume(me, TK_KW_IF);
    RETURN_NULL_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsConditionals);
        UNLOAD_IF_NOT_NULL(clsElse);
    )

    // if we did it -> allocate and return a new node
    ls_if_statement_node_t *node = malloc(sizeof(ls_if_statement_node_t));
    node->base.type = LS_IF_STATEMENT;
    node->lsConditionals = lsConditionals;
    node->clsElse = clsElse;
    node->kwEnd   = kwEnd;
    node->kwEndIf = kwEndIf;
    return (ls_ast_node_t*)node;
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseSelectStatement:
// parse VBAs equivilant to a switch statement
ls_ast_node_t *PARSER_parseSelectStatement(parser_t *me) {

    // expect a Select keyword
    token_t *kwSelect = PARSER_consume(me, TK_KW_SELECT);
    RETURN_NULL_ON_ERROR()

    // expect a Case keyword
    token_t *kwCase = PARSER_consume(me, TK_KW_CASE);
    RETURN_NULL_ON_ERROR()

    // parse the expression we are going to select on
    ls_ast_node_t *exprTest = PARSER_parseExpression(me);
    RETURN_NULL_ON_ERROR(
        UNLOAD_IF_NOT_NULL(exprTest)
    )

    // expect an EOS
    PARSER_ffwToEOS(me);
    RETURN_NULL_ON_ERROR(
        UNLOAD_IF_NOT_NULL(exprTest)
    )

    // create a list for our case conditionals
    ls_ast_node_list_t lsConditionals = PS_LS_AST_NODE_LIST_Init();

    // and a placeholder for our potential else case
    ls_else_clause_node_t *clsElse = NULL;

    // parse select cases until we reach an End keyword
    while (PS_CURRENT().type != TK_KW_END) {

        // is this a Case Else branch?
        if (PS_PEEK(1)->type != TK_KW_ELSE) {

            // nope -> parse a case conditional
            ls_conditional_clause_node_t *clsCase = PARSER_parseConditionalClause(me, TK_KW_CASE);
            RETURN_NULL_ON_ERROR(
                UNLOAD_IF_NOT_NULL(exprTest)
                UNLOAD_IF_NOT_NULL(clsCase)
                PS_LS_AST_NODE_LIST_Unload(lsConditionals);
            )

            // add it to the list
            PS_LS_AST_NODE_LIST_Add(&lsConditionals, (ls_ast_node_t*)clsCase);
        }

        // otherwise, if this is an else case
        else {

            // parse a Case Else block
            clsElse = PARSER_parseElseClause(me);
            RETURN_NULL_ON_ERROR(
                UNLOAD_IF_NOT_NULL(exprTest)
                UNLOAD_IF_NOT_NULL(clsElse)
                PS_LS_AST_NODE_LIST_Unload(lsConditionals);
            )

            // break out of this loop, the else case is always the last one in a select
            break;
        }
    }

    // expect an End Select marker
    token_t *kwEnd = PARSER_consume(me, TK_KW_END);
    RETURN_NULL_ON_ERROR(
        UNLOAD_IF_NOT_NULL(exprTest)
        UNLOAD_IF_NOT_NULL(clsElse)
        PS_LS_AST_NODE_LIST_Unload(lsConditionals);
    )

    token_t *kwEndSelect = PARSER_consume(me, TK_KW_SELECT);
    RETURN_NULL_ON_ERROR(
        UNLOAD_IF_NOT_NULL(exprTest)
        UNLOAD_IF_NOT_NULL(clsElse)
        PS_LS_AST_NODE_LIST_Unload(lsConditionals);
    )

    // if everything's ok, allocate and return this node
    ls_select_statement_node_t *node = malloc(sizeof(ls_select_statement_node_t));
    node->base.type = LS_SELECT_STATEMENT;
    node->kwSelect       = kwSelect;
    node->kwCase         = kwCase;
    node->exprTest       = exprTest;
    node->lsConditionals = lsConditionals;
    node->clsElse        = clsElse;
    node->kwEnd          = kwEnd;
    node->kwEndSelect    = kwEndSelect;
    return (ls_ast_node_t*)node;
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseAssignmentStatement:
// parse a Let or a Set statement
ls_ast_node_t *PARSER_parseAssignmentStatement(parser_t *me, ls_ast_node_t *target) {

    // define placeholders for both the set and the let keyword
    token_t *kwLet = NULL;
    token_t *kwSet = NULL;

    // if we dont have a target already, allow for a let or set keyword
    if (target == NULL) {

        // if theres a let keyword -> consume it
        if (PS_CURRENT().type == TK_KW_LET) {
            kwLet = PARSER_consume(me, TK_KW_LET);
        }

        // if theres a set keyword -> consume it
        if (PS_CURRENT().type == TK_KW_SET) {
            kwSet = PARSER_consume(me, TK_KW_SET);
        }

        // then, parse a target
        target = PARSER_parseExpression(me);
        RETURN_NULL_ON_ERROR()
    }

    // in any case, parse an equals sign
    token_t *opEquals = PARSER_consume(me, TK_OP_EQUALS);
    RETURN_NULL_ON_ERROR()

    // then parse the assignment value
    ls_ast_node_t *exprValue = PARSER_parseExpression(me);
    RETURN_NULL_ON_ERROR()

    // allocate this node and return it
    ls_assignment_statement_node_t *node = malloc(sizeof(ls_assignment_statement_node_t));
    node->base.type = LS_ASSIGNMENT_STATEMENT;
    node->kwLet      = kwLet;
    node->kwSet      = kwSet;
    node->opEquals   = opEquals;
    node->exprTarget = target;
    node->exprValue  = exprValue;

    return (ls_ast_node_t*)node;
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseExpressionStatement:
// some expressions are allowed to be used as statements, like function calls
ls_ast_node_t *PARSER_parseExpressionStatement(parser_t *me) {
    // parse an expression
    ls_ast_node_t *exprExpression = PARSER_parseExpression(me);
    RETURN_NULL_ON_ERROR()

    // wrap it in an expression statement and return it
    ls_expression_statement_node_t *node = malloc(sizeof(ls_expression_statement_node_t));
    node->base.type = LS_EXPRESSION_STATEMENT;
    node->exprExpression = exprExpression;
    return (ls_ast_node_t*)node;
}

// =====================================================================================================================
// EXPRESSIONS
// =====================================================================================================================

ls_ast_node_t *PARSER_parseExpression(parser_t *me) {
    return PARSER_parsePrimaryExpression(me);
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parsePrimaryExpression:
// a primary expression is a sort of expression "atom", it can be something like a number, string, variables, etc
// they can then be combined to form more complex expressions
ls_ast_node_t *PARSER_parsePrimaryExpression(parser_t *me) {
    switch (PS_CURRENT().type) {

        // All literal expressions
        case TK_LT_STRING:
        case TK_LT_NUMBER:
        case TK_LT_BOOLEAN:
            return (ls_ast_node_t*)PARSER_parseLiteralExpression(me);

        // Name or call expressions  are being parsed as reference expressions
        // reference expressions are made out of chains of name and call expressions
        // e.g.: myObject.myField.cry()
        case TK_IDENTIFIER:
            ls_ast_node_t *base = (ls_ast_node_t*)PARSER_parseReferenceExpression(me, NULL);
            RETURN_NULL_ON_ERROR(
                PS_LS_Node_Unload(base);
            )

            // as long as we find a chaining period, parse more reference chain links
            while (PS_CURRENT().type == TK_PC_PERIOD) {
                ls_ast_node_t *newBase = (ls_ast_node_t*)PARSER_parseReferenceExpression(me, base);
                RETURN_NULL_ON_ERROR(
                    PS_LS_Node_Unload(base);
                )

                // the newly parsed reference expression will be the
                base = newBase;
            }

            return base;

        default:;
    }

    // otherwise, we didnt get an expression -> womp, womp
    PS_ERROR_AT(SUB_PARSER, ERR_PS_UNEXPECTED_NON_EXPRESSION, me->source, SPAN_FromToken(PS_CURRENT()), "Expected an expression")
    me->hasError = true;
    return NULL;
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseLiteralExpression:
// parse a literal like a double quoted string, number, or boolean keyword
ls_literal_expression_node_t *PARSER_parseLiteralExpression(parser_t *me) {

    // our actual literal token
    token_t *ltLiteral = NULL;

    // consume the literal token based on its token type
    switch (PS_CURRENT().type) {
        case TK_LT_STRING:
            ltLiteral = PARSER_consume(me, TK_LT_STRING);
        break;
        case TK_LT_NUMBER:
            ltLiteral = PARSER_consume(me, TK_LT_NUMBER);
        break;
        case TK_LT_BOOLEAN:
            ltLiteral = PARSER_consume(me, TK_LT_BOOLEAN);
        break;
        default:;
    }

    // this should never be null after this!
    assert(ltLiteral != NULL);

    // create a new node and return it
    ls_literal_expression_node_t *node = malloc(sizeof(ls_literal_expression_node_t));
    node->base.type = LS_LITERAL_EXPRESSION;
    node->ltLiteral = ltLiteral;
    return node;
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_parseReferenceExpression:
// parse a part of a reference chain, may be a variable access or function call
ls_reference_expression_node_t *PARSER_parseReferenceExpression(parser_t *me, ls_ast_node_t *exprBase) {
    // if theres a period -> consume it
    if (PS_CURRENT().type == TK_PC_PERIOD) {
        PARSER_consume(me, TK_PC_PERIOD);
    }

    // parse the name of this reference chain member
    token_t *idName = PARSER_consume(me, TK_IDENTIFIER);
    RETURN_NULL_ON_ERROR()

    // initialize placeholders for our argument list
    token_t *pcOpenParenthesis = NULL;
    ls_ast_node_list_t lsArguments = PS_LS_AST_NODE_LIST_Init();
    token_t *pcClosedParenthesis = NULL;

    // -----------------------------------------------------------------------------------------------------------------
    // Function call

    // if this is a call expression -> parse the parentheses and the arguments
    if (PS_CURRENT().type == TK_PC_OPEN_PARENTHESIS) {
        pcOpenParenthesis = PARSER_consume(me, TK_PC_OPEN_PARENTHESIS);

        // as long as we are not at the end of the argument list
        while (PS_CURRENT().type != TK_PC_CLOSED_PARENTHESIS) {
            // parse this expression
            ls_ast_node_t *exprArg = PARSER_parseExpression(me);
            RETURN_NULL_ON_ERROR(
                PS_LS_AST_NODE_LIST_Unload(lsArguments);
            )

            // if we're not at the end -> expect a comma
            if (PS_CURRENT().type != TK_PC_CLOSED_PARENTHESIS) {
                PARSER_consume(me, TK_PC_COMMA);
            }
            RETURN_NULL_ON_ERROR(
                PS_LS_AST_NODE_LIST_Unload(lsArguments);
            )

            // add this argument to the list
            PS_LS_AST_NODE_LIST_Add(&lsArguments, exprArg);
        }

        // consume the closing parenthesis
        pcClosedParenthesis = PARSER_consume(me, TK_PC_CLOSED_PARENTHESIS);
    }

    // -----------------------------------------------------------------------------------------------------------------
    // Subroutine call
    else {
        // try parsing an expression after this, if we get one, its probably an argument
        SNAPSHOT(SUB_TRY_ARG)
        ls_ast_node_t *exprArg = PARSER_parseExpression(me);
        ROLLBACK(SUB_TRY_ARG)

        // has this worked?
        if (!me->hasError) {
            PS_LS_Node_Unload(exprArg);

            // as long as we are not at the end of the argument list
            while (PS_CURRENT().type != TK_EOS) {
                // parse this expression
                exprArg = PARSER_parseExpression(me);
                RETURN_NULL_ON_ERROR(
                    PS_LS_AST_NODE_LIST_Unload(lsArguments);
                )

                // if we're not at the end -> expect a comma
                if (PS_CURRENT().type != TK_EOS) {
                    PARSER_consume(me, TK_PC_COMMA);
                }
                RETURN_NULL_ON_ERROR(
                    PS_LS_AST_NODE_LIST_Unload(lsArguments);
                )

                // add this argument to the list
                PS_LS_AST_NODE_LIST_Add(&lsArguments, exprArg);
            }
        }

        //otherwise -> no sub call
        else {
            me->hasError = false;
        }
    }


    RETURN_NULL_ON_ERROR(
        PS_LS_AST_NODE_LIST_Unload(lsArguments);
    )

    // of we've reached this point -> allocate a new node and return it
    ls_reference_expression_node_t *node = malloc(sizeof(ls_reference_expression_node_t));
    node->base.type = LS_REFERENCE_EXPRESSION;
    node->exprBase = exprBase;
    node->idName = idName;
    node->pcOpenParenthesis = pcOpenParenthesis;
    node->lsArguments = lsArguments;
    node->pcClosedParenthesis = pcClosedParenthesis;
    return node;
}

// =====================================================================================================================
// HELPERS
// =====================================================================================================================


// ---------------------------------------------------------------------------------------------------------------------
// PARSER_consume:
// expects and returns a certain kind of token,
// will return null and throw an error when an unexpected token was found
token_t *PARSER_consume(parser_t *me, token_type_t typeOfToken) {
    if (PS_CURRENT().type != typeOfToken) {
        me->hasError = true;
        ERROR_SPLICE_AT(SUB_PARSER, ERR_PS_UNEXPECTED_TOKEN, me->source, SPAN_FromToken(PS_CURRENT()), "Unexpected token (expected: '%s')", TOKEN_TYPE_STRING[typeOfToken])
        return NULL;
    }

    PS_STEP()
    return PS_PEEK(-1);
}

// ---------------------------------------------------------------------------------------------------------------------
// PARSER_ffwToEOS:
// fast forwards the pointer until we find an End Of Statement
// if we arent already there, throw an error once
void PARSER_ffwToEOS(parser_t *me) {
    bool hasReportedError = false;

    // step until we hit an EOS
    while (PS_CURRENT().type != TK_EOS && !AT_EOL()) {
        if (!hasReportedError) {
            hasReportedError = true;
            me->hasError = true;
            ERROR_SPLICE_AT(SUB_PARSER, ERR_PS_UNEXPECTED_TOKEN, me->source, SPAN_FromToken(PS_CURRENT()), "Unexpected token (expected: '%s')", TOKEN_TYPE_STRING[TK_EOS])
        }

        // continue on
        PS_STEP()
    }

    // step over the EOS
    if (!AT_EOL())
        PS_STEP()
}