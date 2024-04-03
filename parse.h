#ifndef PARSE_H
#define PARSE_H

#define MAX_ARGS 64

typedef enum {
    CMD_REGULAR,
    CMD_THEN,
    CMD_ELSE
} CommandType;

typedef struct Command {
    char* argv[MAX_ARGS];
    char* inputFile;
    char* outputFile;
    int pipe;
    char* pipeCommand[MAX_ARGS];
    CommandType type; // Add this line to include conditional command type
} Command;

int parse_command(char* command, Command* cmd);

#endif
