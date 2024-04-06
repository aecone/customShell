// parse.c
#include "parse.h" // Ensure this includes the definition of the Command struct and CommandType
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_command(char* commandLine, Command* cmd) {
    memset(cmd, 0, sizeof(Command)); // Initialize the command structure
    char* token = strtok(commandLine, " ");
    cmd->argv[0] = strdup(token); // Command itself

    // Identify the command type
    if (strcmp(token, "cd") == 0) cmd->type = CMD_CD;
    else if (strcmp(token, "pwd") == 0) cmd->type = CMD_PWD;
    else if (strcmp(token, "exit") == 0) cmd->type = CMD_EXIT;
    else if (strcmp(token, "which") == 0) cmd->type = CMD_WHICH;
    else cmd->type = CMD_EXTERNAL;

    // Parse additional arguments
    int i = 1;
    while ((token = strtok(NULL, " ")) != NULL && i < MAX_ARGS - 1) {
        cmd->argv[i++] = strdup(token);
    }
    cmd->argv[i] = NULL; // Null-terminate the argument list

    return i; // Return the number of arguments, including the command itself
}
