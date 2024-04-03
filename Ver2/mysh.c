#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_COMMAND_LENGTH 1024
#define BUFFER_SIZE 1024

// Function Declarations
void runShell();
void parseAndExecute(char *cmd);
int readCommand(char *buffer, int isInteractive);
void executeCommandWithPipe(char *part1[], char *part2[]);
void parseCommand(char *cmd, char *part1[], char *part2[]);
int executeCommand(char *args[]);

// Main function to decide mode and run the shell
int main() {
    runShell(); // Unified approach for both modes
    return 0;
}

// Unified shell run function
void runShell() {
    char command[MAX_COMMAND_LENGTH] = {0};
    int isInteractive = isatty(STDIN_FILENO);

    if (isInteractive) {
        printf("Welcome to my shell!\n");
    }

    while (1) {
        if (isInteractive) {
            printf("mysh> ");
        }

        if (!readCommand(command, isInteractive)) {
            break; // Exit loop on EOF or if 'exit' command is read
        }

        parseAndExecute(command);
    }

    if (isInteractive) {
        printf("Exiting my shell.\n");
    }
}

// Function to read commands using read()
int readCommand(char *buffer, int isInteractive) {
    memset(buffer, 0, MAX_COMMAND_LENGTH);
    
    char tempChar;
    int bytesRead = 0;
    while (read(STDIN_FILENO, &tempChar, 1) > 0) {
        // Break on newline, indicating end of command
        if (tempChar == '\n') break;
        
        if (bytesRead < MAX_COMMAND_LENGTH - 1) {
            buffer[bytesRead++] = tempChar;
        }
    }

    // Check for EOF or error
    if (bytesRead == 0 && !isInteractive) return 0; // In batch mode, stop on EOF
    if (bytesRead == 0 && isInteractive) return 1; // In interactive mode, prompt again

    buffer[bytesRead] = '\0'; // Null-terminate the command string
    return 1; // Command read successfully
}

void executeCommandWithPipes(char **args, char **pipeArgs) {
    int pipefd[2];
    pid_t pid1, pid2;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        // First child: Executes the first command
        close(pipefd[0]); // Close unused read end
        dup2(pipefd[1], STDOUT_FILENO); // Write to the pipe
        close(pipefd[1]);

        if (execvp(args[0], args) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process
        pid2 = fork();
        if (pid2 == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid2 == 0) {
            // Second child: Executes the second command
            close(pipefd[1]); // Close unused write end
            dup2(pipefd[0], STDIN_FILENO); // Read from the pipe
            close(pipefd[0]);

            if (execvp(pipeArgs[0], pipeArgs) == -1) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        } else {
            // Parent again
            close(pipefd[0]);
            close(pipefd[1]);
            waitpid(pid1, NULL, 0); // Wait for the first child
            waitpid(pid2, NULL, 0); // Wait for the second child
        }
    }
}

void parseAndExecute(char *cmd) {
    char *args[MAX_ARGS];
    char *inputFile = NULL, *outputFile = NULL;

    // Parse the command
    splitCommand(cmd, args, &inputFile, &outputFile);

    // Check for built-in commands or execute external command
    if (!isBuiltInCommand(args[0])) {
        executeExternalCommand(args, inputFile, outputFile);
    } else {
        executeBuiltInCommand(args);
    }

    free(inputFile);
    free(outputFile);
}

int isBuiltInCommand(char *cmd) {
    return strcmp(cmd, "cd") == 0 || strcmp(cmd, "pwd") == 0 || strcmp(cmd, "exit") == 0;
}

void executeBuiltInCommand(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        cmdCd(args);
    } else if (strcmp(args[0], "pwd") == 0) {
        cmdPwd(args);
    } else if (strcmp(args[0], "exit") == 0) {
        cmdExit(args);
    }
}

// Placeholder for external command execution
void executeExternalCommand(char **args, char *inputFile, char *outputFile) {
    int status;
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        if (inputFile) {
            int inFd = open(inputFile, O_RDONLY);
            if (inFd == -1) {
                perror("failed to open input file");
                exit(EXIT_FAILURE);
            }
            dup2(inFd, STDIN_FILENO);
            close(inFd);
        }

        if (outputFile) {
            int outFd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (outFd == -1) {
                perror("failed to open output file");
                exit(EXIT_FAILURE);
            }
            dup2(outFd, STDOUT_FILENO);
            close(outFd);
        }

        if (execvp(args[0], args) == -1) {
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

// Placeholder for command splitting function
void splitCommand(char *cmd, char **args, char **inputFile, char **outputFile) {
    int argCount = 0;
    char *token = strtok(cmd, " ");

    while (token != NULL) {
        if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " ");
            *inputFile = strdup(token);
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " ");
            *outputFile = strdup(token);
        } else {
            args[argCount++] = token;
        }
        token = strtok(NULL, " ");
    }
    args[argCount] = NULL; // Null-terminate the arguments array
}



void parseCommand(char *cmd, char *part1[], char *part2[]) {
    char *token;
    int i = 0;

    // Getting the first part of the pipeline
    token = strtok(cmd, "|");
    while (token != NULL && i < 2) {
        if (i == 0) {
            int j = 0;
            char *subToken = strtok(token, " ");
            while (subToken != NULL) {
                part1[j++] = subToken;
                subToken = strtok(NULL, " ");
            }
            part1[j] = NULL;
        } else {
            int j = 0;
            char *subToken = strtok(token, " ");
            while (subToken != NULL) {
                part2[j++] = subToken;
                subToken = strtok(NULL, " ");
            }
            part2[j] = NULL;
        }
        token = strtok(NULL, "|");
        i++;
    }
}

void executeCommandWithPipe(char *part1[], char *part2[]) {
    int pipefd[2];
    pid_t pid1, pid2;
    int status;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1); // Setting exit status to 1 as per requirement
    }

    pid1 = fork();
    if (pid1 == 0) {
        // Child process 1: Executes the first part
        close(pipefd[0]); // Close unused read end
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to write end of the pipe
        close(pipefd[1]);
        
        execvp(part1[0], part1);
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    pid2 = fork();
    if (pid2 == 0) {
        // Child process 2: Executes the second part
        close(pipefd[1]); // Close unused write end
        dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to read end of the pipe
        close(pipefd[0]);

        execvp(part2[0], part2);
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    // Closing pipe in the parent process
    close(pipefd[0]);
    close(pipefd[1]);

    // Wait for both child processes to finish
    waitpid(pid1, NULL, 0);
    waitpid(pid2, &status, 0); // The exit status of the shell will be the exit status of the last sub-command

    if (WIFEXITED(status)) {
        int exit_status = WEXITSTATUS(status);
        printf("Exit status of the last command was: %d\n", exit_status);
    }
}

int executeCommand(char *args[]) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        // Error forking
        perror("fork");
        return 1;
    } else {
        // Parent process
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return WEXITSTATUS(status);
}

void cmdCd(char **args) {
    if (args[1] == NULL || args[2] != NULL) {
        fprintf(stderr, "cd: wrong number of arguments\n");
    } else if (chdir(args[1]) != 0) {
        perror("cd");
    }
}

void cmdPwd(char **args) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("pwd");
    }
}

void cmdWhich(char **args) {
    if (args[1] == NULL || args[2] != NULL) {
        fprintf(stderr, "which: wrong number of arguments\n");
    } else {
        // Simple example; expand to search /usr/local/bin, /usr/bin, /bin
        printf("/bin/%s\n", args[1]); // Placeholder logic
    }
}