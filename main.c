#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define Assert(condition) if((condition) == false) \
{ fprintf(stderr, "Assertion failed!\nCondtion: %s\nFile: %s\nLine:%d\n", #condition, __FILE__, __LINE__); } 0\

#include "buffer.c"

void BufferTest(void)
{
    int *numbers = NULL;
    int max_number = 2048;
    int i = 0;
    
    while(i < max_number)
    {
        buffer_push(numbers, i);
        i++;
    }
    
    i = 0;
    while(i < max_number)
    {
        Assert(numbers[i] == i);
        i++;
    }
    
}

typedef enum
{
    TOKEN_NUMBER,
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    
    TOKEN_IF,
    TOKEN_WHILE,
    TOKEN_FOR,
    
    TOKEN_EOF
} TokenKind;

static char const *token_string_table[] = {
    [TOKEN_NUMBER] = "number",
    [TOKEN_IDENTIFIER] = "identifier",
    [TOKEN_STRING] = "string",
    
    [TOKEN_IF] = "if",
    [TOKEN_WHILE] = "while",
    [TOKEN_FOR] = "for",
    
    [TOKEN_EOF] = "End of file"
};

typedef struct
{
    TokenKind kind;
    
    union
    {
        int number;
        char name[32];
        char *string;
    };
} Token;

typedef struct
{
    char const *buffer;
    int         current;
} Lexer;

Lexer LexerCreateFromString(char const *string)
{
    Lexer lexer;
    lexer.buffer = string;
    lexer.current = 0;
    
    return lexer;
}

char LexerPeek(Lexer *lexer)
{
    return lexer->buffer[lexer->current];
}

char LexerNext(Lexer *lexer)
{
    char result = LexerPeek(lexer);
    if(result != 0)
    {
        lexer->current++;
    }
    
    return result;
}

Token *LexerRun(Lexer *lexer)
{
    Token *list_of_tokens = NULL;
    Token current_token;
    char c;
    while((c = LexerPeek(lexer)) != 0)
    {
        if(isspace(c))
        {
            LexerNext(lexer);
            continue;
        } else if(isalpha(c) || c == '_')
        {
            int index = 0;
            c = LexerNext(lexer);
            while(isalnum(c) || c == '_')
            {
                current_token.name[index++] = c;
                c = LexerNext(lexer);
            }
            
            current_token.name[index] = 0;
            current_token.kind = TOKEN_IDENTIFIER;
        } else if(isdigit(c))
        {
            int result = 0;
            while(isdigit(c = LexerNext(lexer)))
            {
                result *= 10;
                result += c - '0';
            }
            
            current_token.number = result;
            current_token.kind = TOKEN_NUMBER;
        } else if(c == '"')
        {
            LexerNext(lexer);
            char *start = (char *)&lexer->buffer[lexer->current];
            char *end = start;
            while((c = LexerNext(lexer)) != '"')
            {
                end++;
            }
            
            int string_length = (int)(end - start);
            current_token.string = malloc(string_length + 1);
            strncpy(current_token.string, start, string_length);
            
            current_token.kind = TOKEN_STRING;
        }
        
        buffer_push(list_of_tokens, current_token);
    }
    
    Token eof_token;
    eof_token.kind = TOKEN_EOF;
    
    buffer_push(list_of_tokens, eof_token);
    
    return list_of_tokens;
}

#define TokenAssertIdentifier(token, string) Assert(strncmp(token.name, string, strlen(token.name)) == 0)
#define TokenAssertNumber(token, value) Assert(token.number == value)
#define TokenAssertString(token, string_value) Assert(strncmp(token.string, string_value, strlen(token.string)) == 0)
#define TokenAssertKind(token, token_kind) Assert(token.kind == token_kind)

void LexerTest(void)
{
    Lexer lexer = LexerCreateFromString("test another_test");
    Token *test_tokens = LexerRun(&lexer);
    int token_index = 0;
    
    TokenAssertIdentifier(test_tokens[token_index], "test");
    token_index++;
    TokenAssertIdentifier(test_tokens[token_index], "another_test");
    token_index++;
    TokenAssertKind(test_tokens[token_index], TOKEN_EOF);
    
    lexer = LexerCreateFromString("1 2 3");
    test_tokens = LexerRun(&lexer);
    token_index = 0;
    
    TokenAssertNumber(test_tokens[token_index], 1);
    token_index++;
    TokenAssertNumber(test_tokens[token_index], 2);
    token_index++;
    TokenAssertNumber(test_tokens[token_index], 3);
    token_index++;
    TokenAssertKind(test_tokens[token_index], TOKEN_EOF);
    
    lexer = LexerCreateFromString("\"test string\" \"another test string\"");
    test_tokens = LexerRun(&lexer);
    token_index = 0;
    
    TokenAssertString(test_tokens[token_index], "test string");
    token_index++;
    TokenAssertString(test_tokens[token_index], "another test string");
    token_index++;
    TokenAssertKind(test_tokens[token_index], TOKEN_EOF);
}

int main(void)
{
    BufferTest();
    LexerTest();
}
