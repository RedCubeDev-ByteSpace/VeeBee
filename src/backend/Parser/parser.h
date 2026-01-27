//
// Created by ds on 12/30/25.
//

#ifndef PARSER_H
#define PARSER_H
#include "AST/Loose/ls_ast.h"
#include "Lexer/source.h"
#include "Lexer/token.h"

#include "../AST/Loose/Members/module_member.h"
#include "../AST/Loose/Members/type_member.h"
#include "../AST/Loose/Clauses/as_clause.h"
#include "AST/Loose/Clauses/parameter_clause.h"
#include "AST/Loose/Clauses/conditional_clause.h"
#include "AST/Loose/Clauses/else_clause.h"
#include "AST/Loose/Expressions/literal_expression.h"
#include "AST/Loose/Expressions/reference_expression.h"

// ---------------------------------------------------------------------------------------------------------------------
// Struct for our parser instance
typedef struct PARSER {
    // the source document we are working on
    source_t source;

    // the list of token we are operating on
    token_list_t *tokens;

    // the position we are at in said list
    uint64_t pos;

    // a list of parsed members, which make up this modules global scope
    ls_ast_node_list_t members;

    // have we hit an error while parsing?
    bool hasError;

    // have we turned on error message suppression for this section?
    uint8_t suppressError;

} parser_t;

parser_t *PARSER_Init(source_t source, token_list_t *tokens);
void PARSER_Parse(parser_t *me);
void PARSER_Unload(parser_t *me);

token_t *PARSER_consume(parser_t *me, token_type_t typeOfToken);
void PARSER_ffwToEOS(parser_t *me);

bool PARSER_parseMember(parser_t *me);
bool PARSER_parseModuleMember(parser_t *me);
bool PARSER_parseTypeMember(parser_t *me);
bool PARSER_parseFunctionMember(parser_t *me);
bool PARSER_parseSubMember(parser_t *me);

void PARSER_parseBlockOfStatements(parser_t *me, ls_ast_node_list_t *lsBody, token_type_t *until, uint8_t untilCount);
ls_ast_node_t *PARSER_parseStatement(parser_t *me);
ls_ast_node_t *PARSER_parseDimStatement(parser_t *me);
ls_ast_node_t *PARSER_parseReDimStatement(parser_t *me);
ls_ast_node_t *PARSER_parseGotoStatement(parser_t *me);
ls_ast_node_t *PARSER_parseLabelStatement(parser_t *me);
ls_ast_node_t *PARSER_parseIfStatement(parser_t *me);
ls_ast_node_t *PARSER_parseSelectStatement(parser_t *me);
ls_ast_node_t *PARSER_parseForStatement(parser_t *me);
ls_ast_node_t *PARSER_parseWhileStatement(parser_t *me);
ls_ast_node_t *PARSER_parseDoStatement(parser_t *me);
ls_ast_node_t *PARSER_parseAssignmentStatement(parser_t *me, ls_ast_node_t *target);
ls_ast_node_t *PARSER_parseExpressionStatement(parser_t *me);

ls_as_clause_node_t *PARSER_parseAsClause(parser_t *me, bool functionNotation, bool allowArrayRanges);
ls_parameter_clause_node_t *PARSER_parseParameterClause(parser_t *me);
ls_conditional_clause_node_t *PARSER_parseConditionalClause(parser_t *me, token_type_t expectedConditionalKeyword);
ls_else_clause_node_t *PARSER_parseElseClause(parser_t *me);

ls_ast_node_t *PARSER_parseExpression(parser_t *me);
ls_ast_node_t *PARSER_parsePrimaryExpression(parser_t *me);
ls_literal_expression_node_t *PARSER_parseLiteralExpression(parser_t *me);
ls_reference_expression_node_t *PARSER_parseReferenceExpression(parser_t *me, ls_ast_node_t *exprBase);

// ---------------------------------------------------------------------------------------------------------------------
// Lil helpers
extern token_t PARSER_EOL_Placeholder;

#define AT_EOL() (me->pos >= me->tokens->length - 1)
#define PS_CURRENT() (me->tokens->tokens[me->pos])
#define PS_PEEK(OFFSET) (((me->pos + OFFSET) < me->tokens->length) ? &me->tokens->tokens[me->pos + OFFSET] : &PARSER_EOL_Placeholder)
#define PS_STEP() if (!AT_EOL()) { me->pos++; }
#define PS_STEP_BACK() if (me->pos > 0) { me->pos--; }

#define RETURN_ON_ERROR(FREE_ACTION) \
    if (me->hasError) {              \
        FREE_ACTION                  \
        return false;                \
    }                                \

#define RETURN_NULL_ON_ERROR(FREE_ACTION) \
    if (me->hasError) {                   \
        FREE_ACTION                       \
        return NULL;                      \
    }                                     \

#define BREAK_ON_ERROR(FREE_ACTION) \
    if (me->hasError) {             \
        FREE_ACTION                 \
        break;                      \
    }                               \

#define UNLOAD_IF_NOT_NULL(MEMBER)      \
    if (MEMBER != NULL) PS_LS_Node_Unload((ls_ast_node_t*)(MEMBER)); \

#define SNAPSHOT(ID)       \
    uint64_t ID = me->pos; \
    me->suppressError++;   \

#define ROLLBACK(ID)     \
    me->pos = ID;        \
    me->suppressError--; \

#define PS_ERROR_AT(...)                               \
    if (me->suppressError == 0) ERROR_AT(__VA_ARGS__); \

#define PS_ERROR_SPLICE_AT(...)                               \
    if (me->suppressError == 0) ERROR_SPLICE_AT(__VA_ARGS__); \

#endif //PARSER_H
