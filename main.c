/*
Things to possibly purse:

1. Should the stretchy buffer implementation offer easy iteration?

2. Another we can iterate through an array is using pointers. Increment and then deference to get the next object.
   No need to create a separate type for everything we need to loop over.

3. Error reporting for sure.

4. Complete the lexer.

5. Allow a forcible shutdown of the compilation purely for testing purposes in the error reporting system?
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <ctype.h>

#define Assert(condition) if((condition) == false) \
{ fprintf(stderr, "Assertion failed!\nCondtion: %s\nFile: %s\nLine:%d\n", #condition, __FILE__, __LINE__); } 0\

#include "buffer.c"

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
    int line;
    int column;
    
    union
    {
        int number;
        char name[32];
        char *string;
    };
} Token;

static int errors_reported = 0;
static int max_allowed_errors = 20;

void ReportError(char const *format, ...)
{
    if(errors_reported >= max_allowed_errors)
    {
        fprintf(stderr, "Too many errors!\nMax error limit is %d\n", max_allowed_errors);
        exit(1);
    }
    
    va_list argument_list;
    va_start(argument_list, format);
    vfprintf(stderr, format, argument_list);
    va_end(argument_list);
    
    errors_reported++;
}

Token *LexerRun(char *lexer)
{
    int current_line = 1;
    int current_column = 1;
    Token *list_of_tokens = NULL;
    Token current_token;
    char *token_start = lexer;
    char *token_end = token_start;
    bool add_token = false;
    char c;
    while((c = *(token_start = lexer)) != 0)
    {
        switch(c)
        {
            case ' ':
            case '\t':
            case '\n':
                lexer++;
                
                if(c == '\n')
                {
                    current_line++;
                }
            break;
            
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
            	int result = 0;
            	while(isdigit(c))
            	{
                    result *= 10;
                    result += c - '0';
                    c = *++lexer;
                    
                    if((result * 0.5) >= (INT_MAX * 0.5))
                    {
                        ReportError("[%d : %d] Number literal surpasses what is allowed!\nMax allowed: %d\n", current_line, current_column, INT_MAX);
                    }
                }
            
                current_token.number = result;
                current_token.kind = TOKEN_NUMBER;
                add_token = true;
            }
            break;
            
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
            case 'x':
            case 'y':
            case 'z':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z':
            case '_':
            
            break;
            
            case '"':
            
            break;
            
            default:
                ReportError("[%d : %d] Unrecognized character '%c'\n", current_line, current_column, c);
            break;
        }
        
        token_end = lexer;
        current_column = (int)(token_end - token_start);
        
        if(add_token)
        {
            current_token.line = current_line;
            current_token.column = current_column;
            BufferPush(list_of_tokens, current_token);
            add_token = false;
        }
    }
    
    Token eof_token;
    eof_token.kind = TOKEN_EOF;
    BufferPush(list_of_tokens, eof_token);
    
    return list_of_tokens;
}

#define TokenAssertIdentifier(tokens, string) \
Assert(strncmp(tokens->name, string, strlen(tokens->name)) == 0); tokens++ \

#define TokenAssertNumber(tokens, value) \
Assert(tokens->number == value); tokens++\

#define TokenAssertString(tokens, string_value) \
Assert(strncmp(tokens->string, string_value, strlen(tokens->string)) == 0); tokens++ \

#define TokenAssertKind(tokens, token_kind) \
Assert(tokens->kind == token_kind); tokens++ \

void LexerTest(void)
{
    Token *old_test_tokens_pointer;
    Token *test_tokens = LexerRun("1 2 3 10 20 30");
    old_test_tokens_pointer = test_tokens;
    
    TokenAssertNumber(test_tokens, 1);
    TokenAssertNumber(test_tokens, 2);
    TokenAssertNumber(test_tokens, 3);
    TokenAssertNumber(test_tokens, 10);
    TokenAssertNumber(test_tokens, 20);
    TokenAssertNumber(test_tokens, 30);
    TokenAssertKind(test_tokens, TOKEN_EOF);
    
    BufferFree(old_test_tokens_pointer);
}

void BufferTest(void)
{
    int *numbers = NULL;
    int max_number = 2048;
    int i = 0;
    
    while(i < max_number)
    {
        BufferPush(numbers, i);
        i++;
    }
    
    i = 0;
    while(i < max_number)
    {
        Assert(numbers[i] == i);
        i++;
    }
    
    BufferFree(numbers);
}

int main(void)    
{
    BufferTest();
    LexerTest();
}
