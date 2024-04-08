#include "parse.h" // Ensure this includes the definition of the Command struct and CommandType
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h> // For glob function

void parse_pipeline_command(char *pipelinePart, Command *cmd) {
    char *tempArgs[MAX_ARGS];
    int argIndex = 0; // Index for arguments in the pipeline command
    char *token = strtok(pipelinePart, " \n");

    while (token != NULL && argIndex < MAX_ARGS - 1) {
        tempArgs[argIndex++] = strdup(token);
        token = strtok(NULL, " \n");
    }
    tempArgs[argIndex] = NULL; // Ensure null termination

    // Assuming cmd->pipeCommand is a char** similar to cmd->argv for the first command
    memcpy(cmd->pipeCommand, tempArgs, sizeof(char*) * (argIndex + 1));
}

int parse_command(char* commandLine, Command* cmd) {
    memset(cmd, 0, sizeof(Command)); // Initialize the command structure
    
    char *tempArgs[MAX_ARGS];
    int argIndex = 0; // Index for arguments
 
    // Check for a pipeline in the command
    char* pipelinePart = strchr(commandLine, '|');
    if (pipelinePart) {
        *pipelinePart = '\0'; // Split the command line at the pipeline
        pipelinePart++; // Move to the start of the second command
        cmd->isPipe = 1; // Mark this command as containing a pipeline
    }
    char* token = strtok(commandLine, " \n"); // Including \n to handle newline characters
    // First token is always the command
    tempArgs[argIndex++] = strdup(token); 

    // Identify the command type
    if (strcmp(token, "cd") == 0) cmd->type = CMD_CD;
    else if (strcmp(token, "pwd") == 0) cmd->type = CMD_PWD;
    else if (strcmp(token, "exit") == 0) cmd->type = CMD_EXIT;
    else if (strcmp(token, "which") == 0) cmd->type = CMD_WHICH;
    else cmd->type = CMD_EXTERNAL; // Default to external command

    while (token != NULL) {
        if (strcmp(token, "<") == 0) { // Input redirection
            token = strtok(NULL, " \n");
            if(token) cmd->inputFile = strdup(token);
        } else if (strcmp(token, ">") == 0) { // Output redirection
            token = strtok(NULL, " \n");
            if(token) cmd->outputFile = strdup(token);
        } else if (strchr(token, '*')) { // Wildcard expansion
            glob_t glob_result;
            memset(&glob_result, 0, sizeof(glob_result));

            int glob_flags = GLOB_NOCHECK | GLOB_TILDE;
            glob(token, glob_flags, NULL, &glob_result);

            for (size_t i = 0; i < glob_result.gl_pathc && argIndex < MAX_ARGS - 1; i++) {
                tempArgs[argIndex++] = strdup(glob_result.gl_pathv[i]);
            }
            globfree(&glob_result);
        } else {
            tempArgs[argIndex++] = strdup(token); // Regular argument
        }
        token = strtok(NULL, " \n");
    }
    tempArgs[argIndex] = NULL; // Ensure null termination

    // Copy parsed arguments to cmd->argv, excluding redirections and handling the first part of a pipeline if present
    memcpy(cmd->argv, tempArgs, sizeof(char*) * (argIndex + 1));

    // If a pipeline is present, parse the second part
    if (cmd->isPipe) {
        parse_pipeline_command(pipelinePart, cmd);
    }

    return argIndex; // Number of arguments parsed, excluding redirections
}
