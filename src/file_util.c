#include "file_util.h"
#include "allocator.h"
#include "nums.h"
#include "str_util.h"
#include "xboxkrnl/xboxkrnl.h"
#include <stdio.h>
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
    char *path = xmalloc(name_len + suffix_len + 4 + 1); // "D:\\" + suffix + null terminator
    if (!path) return NULL; // check for allocation failure

    // Construct the path
    strcpy(path, "D:\\");
    strcat(path, name);
    strcat(path, suffix);

    return path;
}

// char **folder_numbered_names(const char *folder, const char *expected_prefix, const char *suffix) {
//     char **names = malloc(100 * sizeof(char*));
//     if (names == NULL) return NULL;
//
//     int n = 0;
//     for (u32 i = 0; i < 100; i++) {
//         char *checker_name = malloc(strlen(folder) + strlen(expected_prefix) + (i <= 9 ? 1 : 2));
//         strcpy(checker_name, folder);
//         strcat(checker_name, expected_prefix);
//         strcat_u32(checker_name, i);
//         char *path = path_name(checker_name, suffix);
//         FILE *file = fopen(path, "rb");
//         if (file != NULL) {
//             // found
//             fclose(file);
//             names[n] = checker_name;
//             n++;
//         } else {
//             // name is of no use :(
//             free(checker_name);
//         }
//         free(path);
//     }
//     names = realloc(names, n * sizeof(char*));
//     return names;
// }
