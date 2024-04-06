// parse.h

#ifndef PARSE_H
#define PARSE_H

#include <stdlib.h> // For size_t definition

// Define a maximum number of arguments a command can have, adjust as needed
#define MAX_ARGS 64

// Enumerate the types of commands your shell can handle
typedef enum {
    CMD_EXTERNAL, // External command
    CMD_CD,       // Change directory
    CMD_PWD,      // Print working directory
    CMD_EXIT,     // Exit the shell
    CMD_WHICH     // Locate a command
    // Add more built-in command types as needed
} CommandType;

// Define the Command structure
typedef struct {
    CommandType type;
    char* argv[MAX_ARGS];   // Arguments for the first command
    int isPipe;             // Flag to indicate if there's a pipeline
    char* pipeCommand[MAX_ARGS]; // Arguments for the second command in the pipeline
    char* inputFile;        // For input redirection
    char* outputFile;       // For output redirection
} Command;
// Function prototypes used in parse.c
int parse_command(char* commandLine, Command* cmd);

#endif // PARSE_H
