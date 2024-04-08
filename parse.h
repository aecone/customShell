#ifndef PARSE_H
#define PARSE_H

#include <stdlib.h> // For size_t definition

// Adjust as needed for your shell's requirements
#define MAX_ARGS 64

typedef enum {
    CMD_EXTERNAL, // External command
    CMD_CD,       // Change directory
    CMD_PWD,      // Print working directory
    CMD_EXIT,     // Exit the shell
    CMD_WHICH     // Locate a command
    // Add more built-in command types as needed
} CommandType;

typedef struct {
    CommandType type;
    char* argv[MAX_ARGS];      // Arguments for the first command
    int isPipe;                // Flag to indicate if there's a pipeline
    char* pipeCommand[MAX_ARGS]; // Arguments for the second command in the pipeline, if present
    char* inputFile;           // For input redirection
    char* outputFile;          // For output redirection
} Command;

// Function prototype for parsing commands
int parse_command(char* commandLine, Command* cmd);

#endif // PARSE_H
