#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "parse.h"    // Include for parsing functionality
#include "execute.h"  // Include for command execution functionality

#define MAX_COMMAND_LENGTH 1024

// Note: No need to redefine MAX_ARGS here, as it's already defined in parse.h

// Forward declaration of additional functions you might need
void read_command(char* buffer);

int main(int argc, char* argv[]) {
    char command[MAX_COMMAND_LENGTH];
    char* args[MAX_ARGS];
    int running = 1;

    // Check for batch mode vs. interactive mode
    if (isatty(STDIN_FILENO)) {
        printf("Welcome to my shell!\n");
    }

    while (running) {
        // Interactive mode prompt
        if (isatty(STDIN_FILENO)) {
            printf("mysh> ");
            fflush(stdout);
        }

        read_command(command);
        if (strcmp(command, "exit") == 0) {
            running = 0; // Exit loop
        } else {
            if (parse_command(command, args) > 0) {
                execute_command(args);
            }
        }
    }

    if (isatty(STDIN_FILENO)) {
        printf("Exiting my shell.\n");
    }
    return 0;
}

void read_command(char* buffer) {
    if (!fgets(buffer, MAX_COMMAND_LENGTH, stdin)) {
        // End of file or error
        if (feof(stdin)) {
            exit(EXIT_SUCCESS); // Normal exit
        } else {
            perror("read_command: fgets");
            exit(EXIT_FAILURE);
        }
    }
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
}
