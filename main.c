/*
1. Support for hexadecimal and octal number literals. (DONE)

2. Add hash map mainly for symbol lookup.

3. Detect integer overflows.

4. Finalize the lexer.

5. Show the number when we overflow.
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
        unsigned int number;
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
            	unsigned int result = 0;
            	unsigned int base = 10;
            	
            	if(c == '0')
            	{
            	    base = 8;
            	    c = *++lexer;
            	    if(tolower(c) == 'x')
            	    {
            	        base = 16;
            	    }
            	}            	
            	
            	while(isalnum(c))
            	{
                    unsigned int digit = 0;
                                        
                    c = tolower(c);
                    if(isdigit(c))
                    {
                        digit = c - '0';
                    } else if(c >= 'a' && c <= 'f')
                    {
                        digit += (c - 'a') + 10;
                    }
                    
                    if(digit > base)
                    {
                        ReportError("Invalid digit in base!\nBase = %u\n", base);
                    }
                    
                    if(result >= ((UINT_MAX - digit) / base))
                    {
                        ReportError("Integer overflow!\n");
                    }
                                        
                    result *= base;
                    result += digit;
                    
                    c = *++lexer;
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
            {
                int index = 0;
                while(isalnum(c) || c == '_')
                {
                    current_token.name[index] = c;
                    index++;
                    c = *++lexer;
                }
                
                current_token.name[index] = 0;
                current_token.kind = TOKEN_IDENTIFIER;
                
                int token_kind = TOKEN_IF;
                while(token_kind < TOKEN_EOF)
                {
                    if(strncmp(current_token.name, token_string_table[token_kind], strlen(current_token.name)) == 0)
                    {
                        current_token.kind = token_kind;
                    }
                    
                    token_kind++;
                }
                
                add_token = true;
            }
            break;
            
            case '"':
            {
                char *start = ++lexer;
                char *end = start;
                c = *lexer;
                while(c != '"')
                {
                    end++;
                    c = *++lexer;
                }
                
                int new_string_length = (int)(end - start);
                char *new_string = malloc(new_string_length + 1);
                strncpy(new_string, start, new_string_length);
                
                current_token.string = new_string;
                current_token.kind = TOKEN_STRING;
                
                lexer++;
                
                add_token = true;
            }
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
    
    test_tokens = LexerRun("number a_string a_string_2 something123");
    old_test_tokens_pointer = test_tokens;
    
    TokenAssertIdentifier(test_tokens, "number");
    TokenAssertIdentifier(test_tokens, "a_string");
    TokenAssertIdentifier(test_tokens, "a_string_2");
    TokenAssertIdentifier(test_tokens, "something123");
    TokenAssertKind(test_tokens, TOKEN_EOF);
    
    BufferFree(old_test_tokens_pointer);
    
    test_tokens = LexerRun("\"test string\" \"another test string\"");
    old_test_tokens_pointer = test_tokens;
    
    TokenAssertString(test_tokens, "test string");
    TokenAssertString(test_tokens, "another test string");
    TokenAssertKind(test_tokens, TOKEN_EOF);
    
    BufferFree(old_test_tokens_pointer);
    
    test_tokens = LexerRun("01 02 03 0123 0567 0xa 0xb 0xc 0xff 0xabcdef 0x7fffffff");
    old_test_tokens_pointer = test_tokens;
    
    TokenAssertNumber(test_tokens, 01);
    TokenAssertNumber(test_tokens, 02);
    TokenAssertNumber(test_tokens, 03);
    TokenAssertNumber(test_tokens, 0123);
    TokenAssertNumber(test_tokens, 0567);
    TokenAssertNumber(test_tokens, 0xa);
    TokenAssertNumber(test_tokens, 0xb);
    TokenAssertNumber(test_tokens, 0xc);
    TokenAssertNumber(test_tokens, 0xff);
    TokenAssertNumber(test_tokens, 0xabcdef);
    TokenAssertNumber(test_tokens, 0x7fffffff);
    TokenAssertKind(test_tokens, TOKEN_EOF);
    
    BufferFree(old_test_tokens_pointer);
    
    //test_tokens = LexerRun("0xfffffffffffff");
    //old_test_tokens_pointer = test_tokens;
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
