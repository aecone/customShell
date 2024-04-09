#include <stdio.h>
#include <stdlib.h>

#define MAX_LENGTH 100

int main() {
    char input[MAX_LENGTH];

    fgets(input, MAX_LENGTH, stdin);

    printf("Pipe: %s", input);

    return 0;
}
