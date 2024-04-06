// main.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "parse.h"

#define MAX_COMMAND_LENGTH 1024

void read_command(char* buffer);
void execute_which(const char* command);

int main(int argc, char* argv[]) {
    char commandLine[MAX_COMMAND_LENGTH];
    Command cmd;
    int running = 1;

    if (isatty(STDIN_FILENO)) {
        printf("Welcome to my shell!\n");
    }

    while (running) {
        if (isatty(STDIN_FILENO)) {
            printf("mysh> ");
            fflush(stdout);
        }

        read_command(commandLine);
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
                printf("%s: command not found\n", cmd.argv[0]);
                break;
        }

        // Free dynamically allocated command arguments
        for (int i = 0; cmd.argv[i] != NULL; i++) {
            free(cmd.argv[i]);
        }
    }

    printf("Exiting my shell.\n");
    return 0;
}

void read_command(char* buffer) {
    fgets(buffer, MAX_COMMAND_LENGTH, stdin);
    if (buffer[strlen(buffer) - 1] == '\n') {
        buffer[strlen(buffer) - 1] = '\0';
    }
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
