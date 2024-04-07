#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "parse.h"
#include "execute.h"

#define MAX_COMMAND_LENGTH 1024

int read_command(int fd, char* buffer);
void execute_which(const char* command);

int main(int argc, char* argv[]) {
    char commandLine[MAX_COMMAND_LENGTH + 1]; // +1 for the null terminator
    Command cmd;
    int running = 1;
    FILE* input_stream = stdin; // Default input is stdin
    int interactive_mode = isatty(STDIN_FILENO) && (argc == 1);

    if (argc == 2) {
        input_stream = fopen(argv[1], "r");
        if (!input_stream) {
            perror("Failed to open file");
            exit(EXIT_FAILURE);
        }
        interactive_mode = 0; // Disable interactive mode if file argument is provided
    }

    if (interactive_mode) {
        printf("Welcome to my shell!\n");
    }

    while (running) {
        if (interactive_mode) {
            printf("mysh> ");
            fflush(stdout);
        }

        read_command(STDIN_FILENO, commandLine);
        if (commandLine[0] == '\0') { // If the command line is empty, skip the rest of the loop
            continue;
        }
        if (!parse_command(commandLine, &cmd)) continue;

        switch (cmd.type) {
            case CMD_CD:
                if (cmd.argv[1] == NULL || cmd.argv[2] != NULL) {
                    fprintf(stderr, "cd: wrong number of arguments\n");
                } else if (chdir(cmd.argv[1]) != 0) {
                    perror("cd");
                }
                break;
            case CMD_PWD:
                {
                    char cwd[1024];
                    if (getcwd(cwd, sizeof(cwd)) != NULL) {
                        printf("%s\n", cwd);
                    } else {
                        perror("pwd");
                    }
                }
                break;
            case CMD_EXIT:
                running = 0;
                break;
            case CMD_WHICH:
                if (cmd.argv[1] != NULL) {
                    execute_which(cmd.argv[1]);
                } else {
                    printf("which: missing argument\n");
                }
                break;
            case CMD_EXTERNAL:
                execute_command(&cmd);
                break;
        }

        // Free dynamically allocated command arguments
        for (int i = 0; cmd.argv[i] != NULL; i++) {
            free(cmd.argv[i]);
        }
    }

    if (interactive_mode) {
        printf("Exiting my shell.\n");
    }
    return 0;
}

int read_command(int fd, char* buffer) {
    int bytesRead = 0;
    char ch;
    while (1) {
        int result = read(fd, &ch, 1);
        if (result > 0) {
            if (ch == '\n') { // Command is complete
                break;
            }
            buffer[bytesRead++] = ch;
            if (bytesRead >= MAX_COMMAND_LENGTH) { // Prevent buffer overflow
                fprintf(stderr, "Command too long.\n");
                bytesRead = 0; // Reset bytesRead to discard the command
                return 1; // Indicate that the program should continue running
            }
        } else {
            // End of file or read error
            if (result == 0) { // End of file
                return 0; // Indicate that the program should stop running
            } else {
                perror("read");
            }
            buffer[bytesRead] = '\0'; // Ensure the buffer is null-terminated
            return 1; // Indicate that the program should continue running
        }
    }
    buffer[bytesRead] = '\0'; // Null-terminate the string
    return 1; // Indicate that the program should continue running
}


void execute_which(const char* command) {
    const char* directories[] = {"/usr/local/bin", "/usr/bin", "/bin"};
    char path[1024];

    for (int i = 0; i < sizeof(directories) / sizeof(directories[0]); i++) {
        snprintf(path, sizeof(path), "%s/%s", directories[i], command);
        if (access(path, X_OK) == 0) {
            printf("%s\n", path);
            return;
        }
    }

    printf("%s not found\n", command);
}
