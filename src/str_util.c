#include "str_util.h"
#include "allocator.h"
#include <string.h>
#include <stdlib.h>

char* strcat_u32(char* dest, u32 value) {
    // Determine the maximum length needed for the integer
    u32 len = 0;
    u32 temp = value;
    if (value == 0) len = 1; // Handle 0 case
    while (temp) {
        len++;
        temp /= 10;
    }

    // Allocate space for the integer as a string (plus null terminator)
    char* num_str = xmalloc(len + 1);
    if (num_str == NULL) return NULL;

    // Convert integer to string manually
    u32 i = len - 1;
    num_str[len] = '\0';
    if (value == 0) num_str[0] = '0';
    else {
        temp = value;
        while (temp) {
            num_str[i--] = temp % 10 + '0';
            temp /= 10;
        }
    }

    // Find the end of the destination string
    size_t dest_len = strlen(dest);

    // Allocate new memory for combined string
    char* result = xrealloc(dest, dest_len + len + 1);
    if (result == NULL) {
        xfree(num_str);
        return NULL;
    }

    // Concatenate the integer string to the destination string
    strcpy(result + dest_len, num_str);

    // Free the temporary string
    xfree(num_str);

    return result;
}
