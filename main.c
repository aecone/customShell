#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 
#include "parse.h"
#include "execute.h"

#define MAX_COMMAND_LENGTH 1024

int read_command(int fd, char* buffer);
void execute_which(const char* command);

int main(int argc, char* argv[]) {
    char commandLine[MAX_COMMAND_LENGTH + 1]; 
    Command cmd;
    int running = 1;
    int fd = STDIN_FILENO; 
    int interactive_mode = isatty(STDIN_FILENO) && (argc == 1);

    if (argc == 2) {
        fd = open(argv[1], O_RDONLY);
        if (fd == -1) {
            perror("Failed to open file");
            exit(EXIT_FAILURE);
        }
        interactive_mode = 0; 
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
        if (!running) break; 
        if (commandLine[0] == '\0') continue; 

        //commands 
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

                for (int i = 1; cmd.argv[i] != NULL; i++) {
                    printf("%s ", cmd.argv[i]);
                }
                if (cmd.argv[1] != NULL) { 
                    printf("\n"); 
                }
                running = 0; 
                break;
            case CMD_WHICH:
                if (cmd.argv[1] != NULL && cmd.argv[2] == NULL) {
                    execute_which(cmd.argv[1]);
                } else{
                    exit(EXIT_FAILURE);
                }
                break;
            case CMD_EXTERNAL:
                execute_command(&cmd);
                break;
        }

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


int read_command(int fd, char* buffer) {
    int bytesRead = 0;
    char ch;
    while (1) {
        ssize_t result = read(fd, &ch, 1);
        
        if (result > 0) {

            if (ch == '\n') {
                break;
            }

            buffer[bytesRead++] = ch;

            if (bytesRead >= MAX_COMMAND_LENGTH) {

                buffer[MAX_COMMAND_LENGTH - 1] = '\0'; 
                fprintf(stderr, "Error: Command too long.\n");
                return 1; 
            }
        } else if (result == 0) {

            if (bytesRead == 0) { 
                return 0;
            }
           
            break; 
        } else {

            perror("read");
            return 0; 
        }
    }

    buffer[bytesRead] = '\0'; 
    return 1; 
}

void execute_which(const char* command) {

    //commands we don't want
    const char* builtins[] = {
        "ls", "cd", "pwd", "touch", "mkdir", "rm", "cp", "mv", "chmod",
        "chown", "df", "exit", "du", "find", "grep", "awk", "sed", "sort", "cat",
        "head", "tail", "less", "more", "echo", "whoami", "uname", "top",
        "ps", "kill", "ssh", "scp", "wget", "curl", "tar", "gzip", "gunzip",
        "zip", "unzip", "mount", "umount", "ifconfig", "ping", "netstat",
        "systemctl", "journalctl", NULL 
    };
    int num_builtins = sizeof(builtins) / sizeof(builtins[0]) - 1; 


    for (int i = 0; i < num_builtins; i++) {
        if (strcmp(command, builtins[i]) == 0) {

            exit(EXIT_FAILURE);
        }
    }

    //path we look thru
    const char* directories[] = {"/usr/local/bin", "/usr/bin", "/bin"};
    int num_directories = sizeof(directories) / sizeof(directories[0]);

    char path[1024];
    int found = 0;

    for (int i = 0; i < num_directories && !found; i++) {
        snprintf(path, sizeof(path), "%s/%s", directories[i], command);
        if (access(path, X_OK) == 0) {
            printf("%s\n", path);
            found = 1;
        }
    }

    if (!found) {
        fprintf(stderr, "%s not found.\n", command);
        exit(EXIT_FAILURE);
    }
}
