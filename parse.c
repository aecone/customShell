#include "parse.h"
#include <string.h>
#include <stdlib.h>

// Breaks down command string into an array of arguments. Returns the number of arguments.
int parse_command(char* command, char** args) {
    int i = 0;
    char* token = strtok(command, " ");
    while (token != NULL) {
        args[i++] = token;
        if (i >= MAX_ARGS) break;
        token = strtok(NULL, " ");
    }
    args[i] = NULL; // Null-terminate the list of arguments
    return i; // Number of arguments
}
