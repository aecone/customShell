#ifndef PARSE_H
#define PARSE_H

#include <stdlib.h> 

#define MAX_ARGS 64

typedef enum {
    CMD_EXTERNAL, 
    CMD_CD,     
    CMD_PWD,     
    CMD_EXIT,    
    CMD_WHICH   
} CommandType;

typedef struct {
    CommandType type;
    char* argv[MAX_ARGS];     
    int isPipe;                
    char* pipeCommand[MAX_ARGS]; 
    char* inputFile;          
    char* outputFile;          
    char *thenCommand[MAX_ARGS];
    char *elseCommand[MAX_ARGS];
    int success; 
} Command;


int parse_command(char* commandLine, Command* cmd);

#endif
