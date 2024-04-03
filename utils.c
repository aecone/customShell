#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>

char **expand_wildcards(char *pattern) {
    DIR *dir;
    struct dirent *entry;
    char **matches = NULL;
    size_t match_count = 0;
    char *slash = strrchr(pattern, '/');
    char *directory, *file_pattern;

    if (slash) {
        directory = strndup(pattern, slash - pattern);
        file_pattern = slash + 1;
    } else {
        directory = strdup(".");
        file_pattern = pattern;
    }

    if ((dir = opendir(directory)) == NULL) {
        perror("opendir");
        free(directory);
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (fnmatch(file_pattern, entry->d_name, 0) == 0) {
            matches = realloc(matches, sizeof(char *) * (++match_count + 1));
            matches[match_count - 1] = strdup(entry->d_name);
        }
    }
    matches[match_count] = NULL;

    closedir(dir);
    free(directory);

    if (match_count == 0) {
        matches = realloc(matches, sizeof(char *) * 2);
        matches[0] = strdup(pattern);
        matches[1] = NULL;
    }

    return matches;
}
