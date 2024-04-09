#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>


void parse_pipeline_command(char *pipelinePart, Command *cmd) {
    char *tempArgs[MAX_ARGS];
    int argIndex = 0; 
    char *token = strtok(pipelinePart, " \n");

    while (token != NULL && argIndex < MAX_ARGS - 1) {
        tempArgs[argIndex++] = strdup(token);
        token = strtok(NULL, " \n");
    }
    tempArgs[argIndex] = NULL; 

    for (int i = 0; i <= argIndex; i++) {
        cmd->pipeCommand[i] = tempArgs[i];
    }
}

int parse_command(char* commandLine, Command* cmd) {
    memset(cmd, 0, sizeof(Command));
    
    char *pipelinePart = strchr(commandLine, '|');
    if (pipelinePart) {
        *pipelinePart = '\0'; 
        pipelinePart++; 
        cmd->isPipe = 1; 
    }

    char *tempArgs[MAX_ARGS]; 
    int argIndex = 0;
    char *token = strtok(commandLine, " \n");
    
    while (token != NULL) {
        if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " \n"); 
            if(token) cmd->inputFile = strdup(token);
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " \n"); 
            if(token) cmd->outputFile = strdup(token);
        } else if (strchr(token, '*')) {
            glob_t glob_result;
            memset(&glob_result, 0, sizeof(glob_result));

            int glob_flags = GLOB_NOCHECK | GLOB_TILDE;
            glob(token, glob_flags, NULL, &glob_result);

            for (size_t i = 0; i < glob_result.gl_pathc && argIndex < MAX_ARGS - 1; i++) {
                tempArgs[argIndex++] = strdup(glob_result.gl_pathv[i]);
            }
            globfree(&glob_result);
        } else {
            tempArgs[argIndex++] = strdup(token);
        }
        token = strtok(NULL, " \n");
    }
    tempArgs[argIndex] = NULL;

    if (argIndex > 0) {
        if (strcmp(tempArgs[0], "cd") == 0) cmd->type = CMD_CD;
        else if (strcmp(tempArgs[0], "pwd") == 0) cmd->type = CMD_PWD;
        else if (strcmp(tempArgs[0], "exit") == 0) cmd->type = CMD_EXIT;
        else if (strcmp(tempArgs[0], "which") == 0) cmd->type = CMD_WHICH;
        else cmd->type = CMD_EXTERNAL;
    }

    for (int i = 0; i <= argIndex; i++) {
        cmd->argv[i] = tempArgs[i];
    }

    if (cmd->isPipe) {
        parse_pipeline_command(pipelinePart, cmd);
    }

    for (int i = 0; i < argIndex; i++) {
        if (strcmp(tempArgs[i], "then") == 0) {

            int thenArgIndex = 0;
            i++; 
            while (i < argIndex && strcmp(tempArgs[i], "else") != 0) {
                cmd->thenCommand[thenArgIndex++] = tempArgs[i];
                i++;
            }
            cmd->thenCommand[thenArgIndex] = NULL;
        } else if (strcmp(tempArgs[i], "else") == 0) {
            
            int elseArgIndex = 0;
            i++; 
            while (i < argIndex) {
                cmd->elseCommand[elseArgIndex++] = tempArgs[i];
                i++;
            }
            cmd->elseCommand[elseArgIndex] = NULL;
        }
    }

    return argIndex;
}
