// Assuming existence of struct Command and related includes
#include "execute.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

void execute_command(Command *cmd) {
    int fd_in, fd_out, pipe_fds[2];
    pid_t pid1, pid2;

    // Input redirection
    if (cmd->inputFile) {
        fd_in = open(cmd->inputFile, O_RDONLY);
        if (fd_in < 0) {
            perror("open input file");
            exit(EXIT_FAILURE);
        }
    }

    // Output redirection
    if (cmd->outputFile) {
        fd_out = open(cmd->outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_out < 0) {
            perror("open output file");
            exit(EXIT_FAILURE);
        }
    }

    // Pipeline
    if (cmd->pipe) {
        if (pipe(pipe_fds) < 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    pid1 = fork();
    if (pid1 == 0) {
        // Child process for the first command or only command
        if (cmd->inputFile) {
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }
        if (cmd->pipe) {
            dup2(pipe_fds[1], STDOUT_FILENO);
        } else if (cmd->outputFile) {
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp first command");
        exit(EXIT_FAILURE);
    }

    if (cmd->pipe) {
        pid2 = fork();
        if (pid2 == 0) {
            // Child process for the second command in the pipeline
            dup2(pipe_fds[0], STDIN_FILENO);
            if (cmd->outputFile) {
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
            close(pipe_fds[1]);
            execvp(cmd->pipeCommand[0], cmd->pipeCommand);
            perror("execvp second command");
            exit(EXIT_FAILURE);
        }
        close(pipe_fds[0]);
        close(pipe_fds[1]);
    }

    // Wait for child processes to finish
    waitpid(pid1, NULL, 0);
    if (cmd->pipe) {
        waitpid(pid2, NULL, 0);
    }
}
