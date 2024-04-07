#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> // For open()
#include "parse.h"
#include "execute.h"

#define MAX_COMMAND_LENGTH 1024

int read_command(int fd, char* buffer);
void execute_which(const char* command);

int main(int argc, char* argv[]) {
        char commandLine[MAX_COMMAND_LENGTH + 1]; // +1 for the null terminator
    Command cmd;
    int running = 1;
    int fd = STDIN_FILENO; // Default to standard input
    int interactive_mode = isatty(STDIN_FILENO) && (argc == 1);

    if (argc == 2) {
        fd = open(argv[1], O_RDONLY);
        if (fd == -1) {
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

        running = read_command(fd, commandLine);
        if (!running) break; // Stop if read_command indicates to stop
        if (commandLine[0] == '\0') continue; // If the command line is empty, skip the rest of the loop

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

    if (fd != STDIN_FILENO) {
        close(fd);
    }

    return 0;
}

// int read_command(int fd, char* buffer) {
//     int bytesRead = 0;
//     char ch;
//     while (read(fd, &ch, 1) > 0) {
//         if (ch == '\n') break; // End of command
//         buffer[bytesRead++] = ch;
//         if (bytesRead == MAX_COMMAND_LENGTH) { // Prevent buffer overflow
//             fprintf(stderr, "Command too long.\n");
//             while (read(fd, &ch, 1) > 0 && ch != '\n'); // Drain the rest of the line
//             break;
//         }
//     }

//     if (bytesRead == 0 && ch != '\n') return 0; // EOF or read error

//     buffer[bytesRead] = '\0'; // Null-terminate the string
//     return 1; // Continue running
// }


int read_command(int fd, char* buffer) {
    int bytesRead = 0;
    char ch;
    while (1) {
        ssize_t result = read(fd, &ch, 1);
        
        if (result > 0) {
            // Check for newline character, indicating the end of a command.
            if (ch == '\n') {
                break;
            }
            // Append the character to the buffer if not newline.
            buffer[bytesRead++] = ch;
            // Ensure we don't exceed the buffer capacity.
            if (bytesRead >= MAX_COMMAND_LENGTH) {
                // Command too long; handle error appropriately.
                buffer[MAX_COMMAND_LENGTH - 1] = '\0'; // Ensure null-termination.
                fprintf(stderr, "Error: Command too long.\n");
                return 1; // Might want to flush the remaining characters.
            }
        } else if (result == 0) {
            // EOF reached, no more input.
            if (bytesRead == 0) { // No command read before EOF.
                return 0;
            }
            // A command was being read, but EOF occurred before newline.
            break; // Proceed to null-terminate and process the command.
        } else {
            // An error occurred during read.
            perror("read");
            return 0; // Return 0 to indicate termination.
        }
    }

    buffer[bytesRead] = '\0'; // Null-terminate the command string.
    return 1; // Command successfully read.
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
