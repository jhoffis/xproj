#include "allocator.h"
#include "hal/debug.h"
#ifdef MEM_TRACK_DBG 

#include "pbkit/pbkit.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    none, standard, aligned, mm
} free_method;

static size_t num_allocations = 0;
static size_t num_real_mem = 0;
static size_t num_dbg_tracking_mem = 0;

static size_t num_max_size = 16;
static void** alloc_ptrs = NULL;
static char** alloc_names = NULL;
static size_t* alloc_sizes = NULL;

void mem_tracker_init(void) {
    const size_t ptr_size = num_max_size * sizeof(void*);
    const size_t names_size = num_max_size * sizeof(char*);
    const size_t sizes_size = num_max_size * sizeof(size_t);
    
    alloc_ptrs = malloc(ptr_size);
    if (!alloc_ptrs) goto fail;
    
    alloc_names = malloc(names_size);
    if (!alloc_names) goto fail;
    
    alloc_sizes = malloc(sizes_size);
    if (!alloc_sizes) goto fail;
    
    num_dbg_tracking_mem += ptr_size + names_size + sizes_size;
    return;

fail:
    free(alloc_ptrs);
    free(alloc_names);
    free(alloc_sizes);
    alloc_ptrs = NULL;
    alloc_names = NULL;
    alloc_sizes = NULL;
    debugPrint("Failed to initialize memory tracker\n");
    exit(1);  // Or handle error differently
}

void mem_tracker_cleanup(void) {
    free(alloc_sizes);
    for (int i = 0; i < num_allocations; i++) {
        free(alloc_names[i]);
    }
    free(alloc_names);
    free(alloc_ptrs);
}

// Function to print how much memory is currently allocated
void print_num_mem_allocated(void) {
    if (num_real_mem > 100000) {
        pb_print("Mem: %zu kb\n", num_real_mem / 1000);
    } else {
        pb_print("Mem: %zu bytes\n", num_real_mem);
    }
    // pb_print("Total tracking memory used: %zu bytes\n", num_dbg_tracking_mem);
    // pb_print("Total memory used (user + tracking): %zu bytes\n", num_real_mem + num_dbg_tracking_mem);
}

/*
 * ==================================
 * ============ TRACKING ============  
 * ==================================
 */

static int upsize_allocation_tracker(void) {
    if (num_allocations < num_max_size) return 1;
    num_max_size *= 2;

    const size_t old_ptr_size = num_allocations * sizeof(void*);
    const size_t old_names_size = num_allocations * sizeof(char*);
    const size_t old_size_size = num_allocations * sizeof(size_t);
    const size_t new_ptr_size = num_max_size * sizeof(void*);
    const size_t new_names_size = num_max_size * sizeof(char*);
    const size_t new_size_size = num_max_size * sizeof(size_t);

    alloc_ptrs = realloc(alloc_ptrs, new_ptr_size);
    alloc_names = realloc(alloc_names, new_names_size);
    alloc_sizes = realloc(alloc_sizes, new_size_size);

    if (alloc_ptrs == NULL || alloc_names == NULL || alloc_sizes == NULL) {
        return NULL;
    }

    num_dbg_tracking_mem += (new_ptr_size   - old_ptr_size) + 
        (new_names_size - old_names_size) + 
        (new_size_size  - old_size_size);
    return 1;
}

static bool track_allocation(void* ptr, size_t size, const char *file, int line) {
    if (!ptr) return false;

    num_allocations++;
    if (!upsize_allocation_tracker()) {
        return false;
    }

    size_t name_size = strlen(file) + 50;
    char *name = malloc(name_size);
    if (!name) {
        return false;
    }
    snprintf(name, name_size, "File: %s, Line: %d", file, line);

    num_real_mem += size;
    alloc_ptrs[num_allocations - 1] = ptr;
    alloc_names[num_allocations - 1] = name;
    alloc_sizes[num_allocations - 1] = size;
    return true;
}


static void untrack_and_free(void* ptr, free_method method) {
    if (ptr == NULL) return;  // Do nothing for NULL pointers

    bool found = false;
    size_t i = 0;  // Iterator for allocations
    for (; i < num_allocations; i++) {
        if (alloc_ptrs[i] == ptr) {
            found = true;
            break;  // Exit loop when allocation is found
        }
    }

    if (!found) {
        debugPrint("Attempt to free unmanaged memory or double free\n");
        return;
    }

    // Free the memory based on the method
    switch (method) {
        case none:
            break;
        case standard:
            free(ptr);
            break;
        case aligned:
            _aligned_free(ptr);
            break;
        case mm:
            MmFreeContiguousMemory(ptr);
            break;
    }

    // Remove the allocation from the tracker
    free(alloc_names[i]);  // Free the associated name
    num_real_mem -= alloc_sizes[i];  // Update memory tracking

    // Shift remaining elements in the tracker to fill the gap
    for (size_t j = i; j < num_allocations - 1; j++) {
        alloc_ptrs[j] = alloc_ptrs[j + 1];
        alloc_names[j] = alloc_names[j + 1];
        alloc_sizes[j] = alloc_sizes[j + 1];
    }

    num_allocations--;  // Decrement the count of tracked allocations
}


/*
 * =====================================
 * ============ ALLOCATIONS ============  
 * =====================================
 */

void *_priv_xmalloc(size_t size, const char *file, int line) {
    void* ptr = malloc(size);
    if (!track_allocation(ptr, size, file, line)) {
        free(ptr);
        return NULL;
    }
    return ptr;
}

void *_priv_x_aligned_malloc(size_t size, size_t alignment, const char *file, int line) {
    void* ptr = _aligned_malloc(size, alignment);
    if (!track_allocation(ptr, size, file, line)) {
        _aligned_free(ptr);
        return NULL;
    }
    return ptr;
}

void *_priv_xcalloc(size_t nmemb, size_t size, const char *file, int line) {
    void* ptr = calloc(nmemb, size);
    size_t total_size = nmemb * size;
    if (!track_allocation(ptr, total_size, file, line)) {
        free(ptr);
        return NULL;
    }
    return ptr;
}

void *_priv_xrealloc(void *old_ptr, size_t size, const char *file, int line) {
    // Attempt reallocation first
    void *new_ptr = realloc(old_ptr, size);
    if (!new_ptr) {
        // Realloc failed; retain the old pointer
        return NULL;
    }

    // Reallocation successful; track new allocation
    if (!track_allocation(new_ptr, size, file, line)) {
        free(new_ptr);  // Free newly allocated memory if tracking fails
        return NULL;
    }

    // Untrack old allocation only after new allocation is successful
    if (old_ptr) {
        untrack_and_free(old_ptr, none);
    }

    return new_ptr;
}

PVOID _priv_xMmAllocateContiguousMemoryEx(SIZE_T NumberOfBytes, ULONG_PTR LowestAcceptableAddress, ULONG_PTR HighestAcceptableAddress, ULONG_PTR Alignment, ULONG Protect, const char *file, int line) {
    PVOID ptr = MmAllocateContiguousMemoryEx(NumberOfBytes, LowestAcceptableAddress, HighestAcceptableAddress, Alignment, Protect);
    if (!track_allocation(ptr, NumberOfBytes, file, line)) {
        MmFreeContiguousMemory(ptr);
        return NULL;
    }
    return ptr;
}

/*
 * =====================================
 * ============ DESTRUCTION ============  
 * =====================================
 */
void xfree(void* ptr) {
    untrack_and_free(ptr, standard);
}

void x_aligned_free(void* memblock) {
    untrack_and_free(memblock, aligned);
}

VOID xMmFreeContiguousMemory(PVOID BaseAddress) {
    untrack_and_free(BaseAddress, mm);
}

#endif
