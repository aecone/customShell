#include <stdio.h>
#include <stdlib.h>

#define MAX_LENGTH 100

int main() {
    char input[MAX_LENGTH];

    // Read input from stdin
    fgets(input, MAX_LENGTH, stdin);

    // Print the input string
    printf("Bro this ain't it: %s", input);

    return 0;
}
