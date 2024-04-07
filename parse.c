#include "parse.h" // Ensure this includes the definition of the Command struct and CommandType
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h> // For glob function

int parse_command(char* commandLine, Command* cmd) {
    memset(cmd, 0, sizeof(Command)); // Initialize the command structure
    
    char *tempArgs[MAX_ARGS];
    int argIndex = 0; // Index for arguments

    char* token = strtok(commandLine, " \n"); // Including \n to handle newline characters
    // First token is always the command
    tempArgs[argIndex++] = strdup(token); 

    // Identify the command type
    if (strcmp(token, "cd") == 0) cmd->type = CMD_CD;
    else if (strcmp(token, "pwd") == 0) cmd->type = CMD_PWD;
    else if (strcmp(token, "exit") == 0) cmd->type = CMD_EXIT;
    else if (strcmp(token, "which") == 0) cmd->type = CMD_WHICH;
    else cmd->type = CMD_EXTERNAL; // Default to external command

    // Parse additional arguments, handling redirection and wildcards
    while ((token = strtok(NULL, " \n")) != NULL) {
        if (strcmp(token, "<") == 0) { // Input redirection
            token = strtok(NULL, " \n"); // Get the file name for input redirection
            if(token != NULL) cmd->inputFile = strdup(token);
        } else if (strcmp(token, ">") == 0) { // Output redirection
            token = strtok(NULL, " \n"); // Get the file name for output redirection
            if(token != NULL) cmd->outputFile = strdup(token);
        } else if (strchr(token, '*')) { // Check for wildcard
            glob_t glob_result;
            memset(&glob_result, 0, sizeof(glob_result));

            int glob_flags = GLOB_NOCHECK | GLOB_TILDE;
            glob(token, glob_flags, NULL, &glob_result);

            for (size_t i = 0; i < glob_result.gl_pathc && argIndex < MAX_ARGS - 1; i++) {
                tempArgs[argIndex++] = strdup(glob_result.gl_pathv[i]);
            }
            globfree(&glob_result);
        } else {
            // Regular argument
            tempArgs[argIndex++] = strdup(token);
        }
    }
    tempArgs[argIndex] = NULL; // Ensure null termination

    // Copy the parsed arguments to the command structure, excluding redirections
    for (int i = 0; i < argIndex; i++) {
        cmd->argv[i] = tempArgs[i];
    }
    cmd->argv[argIndex] = NULL; // Null terminate the array of arguments

    return argIndex; // Number of arguments parsed, excluding redirections
}
