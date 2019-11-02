#include <stdlib.h>

typedef enum
{
    EXPRESSION_NUMBER,
    EXPRESSION_UNARY,
    EXPRESSION_BINARY
} ExpressionKind;

typedef struct Expression
{
    ExpressionKind kind;
    TokenKind      operator;

    union
    {
        int number_expression;
        struct Expression *unary_expression;

        struct
        {
            struct Expression *left_expression;
            struct Expression *right_expression;
        };
    };
} Expression;

bool MatchToken(Token **tokens, TokenKind kind, Token *matched_on_token)
{
    if((*tokens)->kind == kind)
    {
        if(matched_on_token) *matched_on_token = *((*tokens)++);
        return true;
    }

    return false;
}

Expression *CreateExpression(ExpressionKind kind)
{
    Expression *expression = malloc(sizeof *expression);
    expression->kind = kind;

    return expression;
}

Expression *CreateNumberExpression(int number)
{
    Expression *expression = CreateExpression(EXPRESSION_NUMBER);
    expression->number_expression = number;

    return expression;
}

Expression *CreateUnaryExpression(Expression *other_expression, TokenKind operator)
{
    Expression *expression = CreateExpression(EXPRESSION_UNARY);
    expression->operator = operator;
    expression->unary_expression = other_expression;

    return expression;
}

Expression *CreateBinaryExpression(Expression *left_expression, Expression *right_expression, TokenKind operator)
{
    Expression *expression = CreateExpression(EXPRESSION_BINARY);
    expression->operator = operator;
    expression->left_expression = left_expression;
    expression->right_expression = right_expression;

    return expression;
}

int EvaluateExpression(Expression *expression)
{
    int result;
    switch(expression->kind)
    {
        case EXPRESSION_NUMBER:
            result = expression->number_expression;
            break;
        case EXPRESSION_UNARY:
        {
            int evaluated_expression = EvaluateExpression(expression->unary_expression);
            switch(expression->operator)
            {
                case TOKEN_MINUS:
                    evaluated_expression = -evaluated_expression;
                    break;
                default:
                    break;
            }
            result = evaluated_expression;
        }
        break;

        default:
            break;
    }

    return result;
}

Expression *ParseExpression(Token **tokens)
{
    Expression expression;
    Expression *result;
    Token token_number;
    if(MatchToken(tokens, TOKEN_MINUS, NULL))
    {
        expression.operator = TOKEN_MINUS;
        (*tokens)++;
        return ParseExpression(tokens);
    }

    if(MatchToken(tokens, TOKEN_NUMBER, &token_number))
    {
        result = CreateNumberExpression(token_number.number);
    }

    return result;
}

void PrettyPrintExpression(Expression *expression)
{
    printf("( ");
    switch(expression->kind)
    {
        case EXPRESSION_NUMBER:
            printf("%d ", expression->number_expression);
            break;
        case EXPRESSION_UNARY:
            switch(expression->operator)
            {
                case TOKEN_MINUS:
                    printf("- ");
                    break;
                default:
                    break;
            }
            PrettyPrintExpression(expression->unary_expression);
            break;

        default:
            break;
    }

    printf(")");
}
