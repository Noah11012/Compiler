#include <stdlib.h>

typedef enum
{
    EXPRESSION_NONE,
    EXPRESSION_NUMBER,
    EXPRESSION_UNARY,
    EXPRESSION_BINARY
} ExpressionKind;

static char *expression_kind_string_table[] = {
    [EXPRESSION_NONE] = "None",
    [EXPRESSION_NUMBER] = "Number",
    [EXPRESSION_UNARY] = "Unary",
    [EXPRESSION_BINARY] = "Binary"
};

static char token_operator_string_table[] = {
    [TOKEN_PLUS] = '+',
    [TOKEN_MINUS] = '-'
};

typedef struct Expression
{
    ExpressionKind kind;
    TokenKind      operator;

    union
    {
        int number;
        struct Expression *unary;

        struct
        {
            struct Expression *left;
            struct Expression *right;
        };
    };
} Expression;

static Token global_token;

bool MatchToken(Token **tokens, TokenKind kind)
{
    if((*tokens)->kind == kind)
    {
        global_token = *((*tokens)++);
        return true;
    }

    return false;
}

bool MatchMultipleTokens(Token **tokens, TokenKind *kinds, int count)
{
    bool result;
    int i = 0;
    while(i < count)
    {
        result = MatchToken(tokens, kinds[i]);
        if(!result)
        {
            break;
        }

        i++;
    }

    return result;
}

void DemandToken(Token **tokens, TokenKind kind)
{
    if(!MatchToken(tokens, kind))
    {
        Assert(0);
    }
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
    expression->number = number;

    return expression;
}

Expression *CreateUnaryExpression(Expression *other_expression, TokenKind operator)
{
    Expression *expression = CreateExpression(EXPRESSION_UNARY);
    expression->operator = operator;
    expression->unary = other_expression;

    return expression;
}

Expression *CreateBinaryExpression(Expression *left_expression, Expression *right_expression, TokenKind operator)
{
    Expression *expression = CreateExpression(EXPRESSION_BINARY);
    expression->operator = operator;
    expression->left = left_expression;
    expression->right = right_expression;

    return expression;
}

Expression *ParseNumber(Token **tokens)
{
    Expression *expression = NULL;

    DemandToken(tokens, TOKEN_NUMBER);

    expression = CreateNumberExpression(global_token.number);

    return expression;
}

Expression *ParseAdditionAndSubtraction(Token **tokens)
{
    Expression *result;
    Expression *number1 = ParseNumber(tokens);

    while(MatchToken(tokens, TOKEN_PLUS))
    {
        result = CreateBinaryExpression(number1, ParseNumber(tokens), TOKEN_PLUS);
    }

    return result;
}

char *FormatString(char const *format, ...)
{
    char *result;

    va_list list;
    va_start(list, format);
    int size = vsnprintf(NULL, 0, format, list);
    result = malloc(size + 1);
    Assert(result);
    vsnprintf(result, size + 1, format, list);
    va_end(list);

    return result;
}

char *StringifyExpression(Expression *expression)
{
    StringBuilder expression_builder = CreateStringBuilder();

    switch(expression->kind)
    {
        case EXPRESSION_NONE:
            break;
        case EXPRESSION_NUMBER:
            PushToStringBuilder(&expression_builder, "%d", expression->number);
            break;
        case EXPRESSION_UNARY:
            PushToStringBuilder(&expression_builder, "(%c %s)",
                    token_operator_string_table[expression->operator],
                    StringifyExpression(expression->unary));
            break;
        case EXPRESSION_BINARY:
            PushToStringBuilder(&expression_builder, "(%c %s %s)",
                    token_operator_string_table[expression->operator],
                    StringifyExpression(expression->left),
                    StringifyExpression(expression->right));
            break;
    }

    return FinalizeStringBuilder(&expression_builder);
}
