#include "parse.h"
#include "utils.h" // Include the header where expand_wildcards is declared
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_command(char* command, Command* cmd) {
    memset(cmd, 0, sizeof(Command)); // Initialize the command structure
    char* token = strtok(command, " ");

    if (strcmp(token, "then") == 0) {
        cmd->type = CMD_THEN;
        token = strtok(NULL, " "); // Move to the next token after "then"
    } else if (strcmp(token, "else") == 0) {
        cmd->type = CMD_ELSE;
        token = strtok(NULL, " "); // Move to the next token after "else"
    } else {
        cmd->type = CMD_REGULAR;
    }
    
    int i = 0;
    while (token) {
        // Check for and handle redirections and pipes as before
        // For simplicity, this part is omitted; focus on wildcard expansion

        // Expand wildcards, if any
        char **expansions = expand_wildcards(token);
        if (expansions) {
            // Add all expansions to the command's arguments
            for (int j = 0; expansions[j] != NULL; j++) {
                if (i >= MAX_ARGS - 1) break; // Prevent overflow
                cmd->argv[i++] = expansions[j];
            }
            free(expansions); // Remember to free the expansions array itself
        } else {
            // No expansion needed; add the token as-is
            cmd->argv[i++] = strdup(token);
        }

        token = strtok(NULL, " ");
    }
    cmd->argv[i] = NULL; // Null-terminate the list of arguments
    return i; // Return the number of arguments
}
