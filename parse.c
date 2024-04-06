#include "parse.h" // Ensure this includes the definition of the Command struct and CommandType
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_command(char* commandLine, Command* cmd) {
    memset(cmd, 0, sizeof(Command)); // Initialize the command structure
    
    // Temporary storage for command arguments to support redirection parsing
    char *tempArgs[MAX_ARGS];
    int argIndex = 0; // Index for arguments, excluding redirection tokens and file paths

    char* token = strtok(commandLine, " ");
    tempArgs[argIndex++] = strdup(token); // Command itself

    // Identify the command type
    if (strcmp(token, "cd") == 0) cmd->type = CMD_CD;
    else if (strcmp(token, "pwd") == 0) cmd->type = CMD_PWD;
    else if (strcmp(token, "exit") == 0) cmd->type = CMD_EXIT;
    else if (strcmp(token, "which") == 0) cmd->type = CMD_WHICH;
    else cmd->type = CMD_EXTERNAL;

    // Parse additional arguments, handling redirection
    while ((token = strtok(NULL, " ")) != NULL) {
        if (strcmp(token, "<") == 0) { // Input redirection
            token = strtok(NULL, " "); // Get the file name for input redirection
            if(token != NULL) cmd->inputFile = strdup(token);
        } else if (strcmp(token, ">") == 0) { // Output redirection
            token = strtok(NULL, " "); // Get the file name for output redirection
            if(token != NULL) cmd->outputFile = strdup(token);
        } else {
            // Regular argument
            if (argIndex < MAX_ARGS - 1) {
                tempArgs[argIndex++] = strdup(token);
            }
        }
    }
    tempArgs[argIndex] = NULL; // Null-terminate the temporary argument list

    // Copy over arguments to cmd->argv, excluding redirection tokens and file paths
    for(int i = 0; i < argIndex; i++) {
        cmd->argv[i] = tempArgs[i];
    }
    cmd->argv[argIndex] = NULL; // Ensure the cmd->argv is also null-terminated

    return argIndex; // Return the number of arguments, excluding redirections
}
