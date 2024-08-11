/*Contains an implementation of a simple interpreter supporting variables
and arbitrarily nested mathematical operations.

This header provides the core functions initInterpreter, evaluate, and closeInterpreter,
the first and last of which should be called to setup and teardown the interpreter to avoid
memory leaks.

Note that this interpreter comes with a few constraints:
- it only understands integers and not float and thus does not support true division
- it only supports up to 100 variables, with names up to 254 chars long.
- it uses 0xdeadbeef as an undefined value and thus will not work as expected around that value.
- it supports negative numbers with some caveats:
    - leading minus sign is ok: -1+2 ==> 1
    - negative numbers in calculations should be surrounded by parentheses, e.g. 2*(-3) ==> -6
*/

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NO_VARIABLE ""
#define UNDEFINED_VALUE (const int)0xdeadbeef /* HACK */
#define UNDEFINED_OPERATOR '#'
#define MAX_VARIABLES 10

/* 254 leaves space for final null-terminator */
#define MAX_VARIABLE_NAME_LENGTH 254

/* Status codes */
enum StatusCodes
{
    OK_CODE = 0,    /* Success */
    EMPTY_CODE = 1, /* Input string consists entirely of whitespaces */
    INVALID_OPERATOR = 2,
    UNDEFINED_VARIABLE = 3,
    INVALID_INPUT = 4,
    DIVISION_BY_ZERO = 5,
};

typedef struct
{
    const char *name;
    int value;
} Variable;

Variable *variables[MAX_VARIABLES];
int _currentVariableCount;
int _scopeCount;

/* Initializes the interpreter */
int initInterpreter(void)
{
    _scopeCount = 0;
    _currentVariableCount = 0;
    return OK_CODE;
}

/* Closes the interpreter and frees memory. */
void closeInterpreter(void)
{
    for (int i = 0; i < MAX_VARIABLES; i++)
    {
        Variable *variable = variables[i];
        if (variable == NULL)
            break;
        free(variable->name);
        free(variable);
    }
    _currentVariableCount = 0;
    _scopeCount = 0;
}

bool isDigit(const char character)
{
    return character >= '0' && character <= '9';
}

bool isCharacter(const char character)
{
    return (character >= 'a' && character <= 'z') || (character > 'A' && character < 'Z') || character == '_' || isDigit(character);
}

/* Sets the value of the variable specified by name.
Note: currently modelled as a fixed-size Array for convenience. In a more performant version,
this should be replaced by a hash-map.
*/
void setVariable(const char *name, int value)
{
    Variable *variable;
    for (int i = 0; i < MAX_VARIABLES; i++)
    {
        Variable *variable = variables[i];
        if (variable == NULL)
            break;
        if (strncmp(variable->name, name, MAX_VARIABLE_NAME_LENGTH) == 0)
        {
            variable->value = value;
            return;
        }
    }

    variable = malloc(sizeof(Variable));
    variable->name = name;
    variable->value = value;
    variables[_currentVariableCount] = variable;
    _currentVariableCount++;
}

/* Sets value pointer if return code is 0, else errors out.
Note: currently modelled as a fixed-size Array for convenience. In a more performant version,
this should be replaced by a hash-map.
*/
enum StatusCodes getVariable(const char *name, int *value)
{
    for (int i = 0; i < MAX_VARIABLES; i++)
    {
        Variable *variable = variables[i];
        if (variable == NULL)
            break;
        if (strncmp(variable->name, name, MAX_VARIABLE_NAME_LENGTH) == 0)
        {
            *value = variable->value;
            return OK_CODE;
        }
    }
    return UNDEFINED_VARIABLE;
}

/* Returns -1 if specified char is not the next non-whitespace char, else returns offset from start. */
int peek(const char *input, const char target)
{
    int lookahead = 0;
    while ((unsigned char)*(input + lookahead))
    {
        char character = (unsigned char)*(input + lookahead);
        lookahead++;
        if (isspace(character))
            continue;
        if (character == target)
            return lookahead - 1;
    }
    return -1;
}

enum StatusCodes calculate(const int a, const char operator, const int b, int *result)
{
    if ((operator== '/' || operator== '%') && b == 0)
        return DIVISION_BY_ZERO;
    switch (operator)
    {
    case '+':
        *result = a + b;
        break;
    case '-':
        *result = a - b;
        break;
    case '*':
        *result = a * b;
        break;
    case '/':
        *result = a / b;
        break;
    case '%':
        *result = a % b;
        break;
    default:
        return INVALID_OPERATOR;
    }
    return OK_CODE;
}

/* In-place evaluation of the specified string input without tokenizing and parsing
into an abstract syntax tree. */
int evaluateInner(char *input, int *result, const int scope)
{
    int tempResult[] = {UNDEFINED_VALUE};
    int number[] = {UNDEFINED_VALUE};
    char operator= UNDEFINED_OPERATOR;
    char *variable = NO_VARIABLE;
    bool negated = false;
    while ((unsigned char)*input)
    {
        char character = (unsigned char)*input;
        bool isNumber = false;

        // skip whitespace
        if (isspace(character))
        {
            input++;
            continue;
        }

        // parse number
        int i = 0;
        if (isDigit(character))
            *number = 0;
        while (isDigit(character))
        {
            *number = (*number) * 10 + (character - '0');
            i++;
            input++;
            character = (unsigned char)*input;
        }
        isNumber = i > 0;

        // check variable
        if (isCharacter(character))
        {
            variable = malloc(sizeof(char) * MAX_VARIABLE_NAME_LENGTH + 1); // cleaned up in closeInterpreter
            int i = 0;
            while (isCharacter(character))
            {
                variable[i] = character;
                i++;
                input++;
                character = (unsigned char)*input;
            }
            variable[i] = '\0';

            // handle assignment if next non-whitespace char is '='
            const int offset = peek(input, '=');
            if (offset != -1)
            {
                int statusCode = evaluateInner(input + offset + 1, tempResult, scope);
                if (statusCode != OK_CODE)
                    return statusCode == EMPTY_CODE ? INVALID_INPUT : statusCode;
                setVariable(variable, *tempResult);
                *result = *tempResult;
                return OK_CODE;
            }
            // lookup variable value if no assignment
            int statusCode = getVariable(variable, number);
            if (statusCode != OK_CODE)
                return UNDEFINED_VARIABLE;
            isNumber = true;
        }

        // handle new scope with a recursive call
        if (character == '(')
        {
            _scopeCount++;
            int statusCode = evaluateInner(input + 1, number, scope + 1);
            if (statusCode != OK_CODE)
                return statusCode == EMPTY_CODE ? INVALID_INPUT : statusCode;
            isNumber = true;

            // advance input pointer until matching closing parenthesis is found
            int brackets = 1;
            input++; // skip leading parenthesis
            while ((unsigned char)*input)
            {
                character = (unsigned char)*input;
                if (character == ')')
                    brackets--;
                else if (character == '(')
                    brackets++;
                input++;
                if (!brackets)
                    break;
            }
        }

        // calculate with number
        if (isNumber)
        {
            if (*tempResult != UNDEFINED_VALUE)
            {
                int initial = *tempResult;
                int statusCode = calculate(initial, operator, * number, tempResult);
                if (statusCode != OK_CODE)
                    return statusCode;
                operator= UNDEFINED_OPERATOR;
            }
            else
                *tempResult = (negated ? -1 : 1) * (*number);
            if (negated)
                negated = false;
            continue;
        }

        // exit scope
        if (character == ')')
        {
            _scopeCount--;
            break;
        }

        if (*number == UNDEFINED_VALUE || operator!= UNDEFINED_OPERATOR)
        {
            if (character != '-')
                return INVALID_INPUT;
            // leading minus sign
            negated = !negated;
            input++;
            continue;
        }

        operator= character;
        // instead of a recursive call, higher-precedence operators can continue in this loop
        if (operator== '*' || operator== '/' || operator== '%')
        {
            input++;
            continue;
        }

        if (operator== '-')
        {
            operator= '+';
            input--; // keep leading minus sign in recursive call
        }
        int statusCode = evaluateInner(input + 1, number, scope);
        if (statusCode != OK_CODE)
            return statusCode == EMPTY_CODE ? INVALID_INPUT : statusCode;
        int initial = *tempResult;
        statusCode = calculate(initial, operator, * number, tempResult);
        if (statusCode != OK_CODE)
            return statusCode;
        operator= UNDEFINED_OPERATOR;
        break;
    }

    if (*tempResult == UNDEFINED_VALUE)
        return EMPTY_CODE; // no input
    if (operator!= UNDEFINED_OPERATOR)
        return INVALID_INPUT; // incomplete statement

    *result = *tempResult;
    return OK_CODE;
}

/* evaluate: evaluate the string expression, and return a status code
   (any value other than OK_CODE and EMPTY_CODE is treated as an error).
   The result of evaluating the expression is placed in a variable
   by the pointer 'result' if the function returns OK_CODE. */
int evaluate(char *input, int *result)
{
    _scopeCount = 0;
    int a[] = {0};
    evaluateInner("1 + -559038737", a, 0);
    int status = evaluateInner(input, result, 0);
    if (_scopeCount != 0)
        return INVALID_INPUT;
    return status;
}
