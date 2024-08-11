#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "interpreter.h"

int MAX_INPUT_LENGTH = 1000;

/* Reads a null-terminated line into the specified buffer. If
a newline char isn't found before the specified maxlen, returns -1,
else returns 0.
*/
int getline(char *buffer, int maxlen)
{
    int i = 0;
    char c;
    while (i < maxlen)
    {
        c = getchar();
        buffer[i] = c;
        if (c == '\n')
        {
            buffer[i] = '\0';
            return 0;
        }
        i++;
    }
    return -1;
}

/* Prints the interpreter help doc */
void printHelp()
{
    printf("A simple interpreter that can calculate arbitrarily nested mathematical ");
    printf("expressions.\nIt supports:\n");
    printf("- operators +, -, *, /, %%.\n");
    printf("- variable assignments, e.g. x = 1 or x = 1 + 1 (max 100 variables)\n");
    printf("- variable value retrieval\n");
    printf("- operation nesting\n");
    printf("Note that it is integer-based and does not understand floating point numbers");
    printf(" or float division.\n");
}

/* Handles StatusCodes defined in interpreter.h */
int handleErrorStatus(const int status, const int *result)
{
    switch (status)
    {
    case 0:
        printf("%i\n", *result);
        break;
    case 1:
        break;
    case 2:
        printf("Invalid operator\n");
        break;
    case 3:
        printf("Undefined variable\n");
        break;
    case 4:
        printf("Invalid input\n");
        break;
    case 5:
        printf("Division by zero\n");
        break;
    default:
        printf("Error occured\n");
        break;
    }
}

/* A simple REPL interpreter for integer operations. */
int main()
{
    char *buffer = malloc(sizeof(char) * MAX_INPUT_LENGTH);
    size_t size;
    int status = 0;
    int result[] = {0};
    initInterpreter();
    printf("Simple REPL shell. Type \"help\" for help, or \"exit\" to exit the shell.\n");
    while (true)
    {
        printf(">> ");
        status = getline(buffer, MAX_INPUT_LENGTH);
        if (status == -1)
            continue;
        if (strncmp(buffer, "exit", MAX_INPUT_LENGTH) == 0)
            break;
        if (strncmp(buffer, "help", MAX_INPUT_LENGTH) == 0)
        {
            printHelp();
            continue;
        }
        status = evaluate(buffer, result);
        handleErrorStatus(status, result);
    }
    closeInterpreter();
    free(buffer);
    return 0;
}
