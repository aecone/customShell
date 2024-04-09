#include "execute.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int execute_with_search(char *const argv[]) {
    const char *directories[] = {"/usr/local/bin", "/usr/bin", "/bin"};
    char path[1024];

    for (int i = 0; i < sizeof(directories) / sizeof(char *); i++) {
        snprintf(path, sizeof(path), "%s/%s", directories[i], argv[0]);
        execv(path, argv);
    }

    return 0;
}

void execute_command(Command *cmd) {
    int pipe_fds[2];
    pid_t pid1, pid2;
    int fd_in = -1, fd_out = -1;

    if (cmd->inputFile) {
        fd_in = open(cmd->inputFile, O_RDONLY);
        if (fd_in < 0) {
            perror("Failed to open input file");
            if (cmd->elseCommand[0]) {

                pid_t pid = fork();
                if (pid == 0) { 

                    execvp(cmd->elseCommand[0], cmd->elseCommand);

                    perror("Execution failed");
                    exit(EXIT_FAILURE);
                } else if (pid > 0) {

                    waitpid(pid, NULL, 0);
                } else {

                    perror("Fork failed");
                }
            }
            return;
        }
    }


    if (cmd->outputFile) {
        fd_out = open(cmd->outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0640);
        if (fd_out < 0) {
            perror("Failed to open output file");
            if (fd_in != -1) close(fd_in);
                if (cmd->elseCommand[0]) {

                    pid_t pid = fork();
                    if (pid == 0) { 

                        execvp(cmd->elseCommand[0], cmd->elseCommand);

                        perror("Execution failed");
                        exit(EXIT_FAILURE);
                    } else if (pid > 0) {

                        waitpid(pid, NULL, 0);
                    } else {

                        perror("Fork failed");
                    }
                }

            return;
        }
    }


    if (cmd->isPipe) {
        if (pipe(pipe_fds) < 0) {
            perror("Failed to create pipe");
            if (fd_in != -1) close(fd_in);
            if (fd_out != -1) close(fd_out);
            if (cmd->elseCommand[0]) {

                    pid_t pid = fork();
                    if (pid == 0) { 

                        execvp(cmd->elseCommand[0], cmd->elseCommand);

                        perror("Execution failed");
                        exit(EXIT_FAILURE);
                    } else if (pid > 0) {

                        waitpid(pid, NULL, 0);
                    } else {

                        perror("Fork failed");
                    }
            }
            return;
        }
    }


    pid1 = fork();
    if (pid1 == 0) { 

        if (fd_in != -1) {
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        if (cmd->isPipe) {
            dup2(pipe_fds[1], STDOUT_FILENO);
            close(pipe_fds[0]);
            close(pipe_fds[1]);
        } else if (fd_out != -1) { 
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }
        if (execute_with_search(cmd->argv) == 0) {
            execvp(cmd->argv[0], cmd->argv);
        }
        
        perror("Execution failed");
        exit(EXIT_FAILURE);
    }

    if (cmd->isPipe) {
        pid2 = fork();
        if (pid2 == 0) { 
            dup2(pipe_fds[0], STDIN_FILENO);
            if (fd_out != -1) { 
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
            close(pipe_fds[0]);
            close(pipe_fds[1]);
            execvp(cmd->pipeCommand[0], cmd->pipeCommand);
            perror("Execution failed");
            exit(EXIT_FAILURE);
        }
        close(pipe_fds[0]);
        close(pipe_fds[1]);
    }

    int status;
    waitpid(pid1, &status, 0); 
    cmd->success = WIFEXITED(status) && WEXITSTATUS(status) == 0;
    
    if (cmd->success && cmd->thenCommand[0]) {

        execvp(cmd->thenCommand[0], cmd->thenCommand);
    } else if (!cmd->success && cmd->elseCommand[0]) {

        execvp(cmd->elseCommand[0], cmd->elseCommand);
    }


    if (pid1 > 0) waitpid(pid1, NULL, 0);
    if (cmd->isPipe && pid2 > 0) waitpid(pid2, NULL, 0);

    if (fd_in != -1) close(fd_in);
    if (fd_out != -1) close(fd_out);
}
