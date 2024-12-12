#include "file_util.h"
#include <stdlib.h>
#include <string.h>


char* path_name(const char *name, const char* suffix) {
    size_t name_len = strlen(name);
    if (name_len > 32) return NULL;
    size_t suffix_len = strlen(suffix);
    if (suffix_len < 2) return NULL;

    for (int i = 0; i < name_len; i++) {
        if ((name[i] >= 'A' && name[i] <= 'Z') ||
            (name[i] >= 'a' && name[i] <= 'z') ||
            (name[i] >= '0' && name[i] <= '9') ||
            name[i] == '(' || 
            name[i] == ')' || 
            name[i] == '.' || 
            name[i] == '\0' || 
            name[i] == ' '  || 
            name[i] == '_') {
            continue;
        }
        return NULL;
    }
    char *path = malloc(name_len + suffix_len + 4 + 1); // "D:\\" + suffix + null terminator
    if (!path) return NULL; // check for allocation failure

    // Construct the path
    strcpy(path, "D:\\");
    strcat(path, name);
    strcat(path, suffix);

    return path;
}
