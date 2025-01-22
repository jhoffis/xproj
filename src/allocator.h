#pragma once
#include <stdlib.h>
#include "nums.h"
#include "xboxkrnl/xboxkrnl.h"

#define MEM_TRACK_DBG
#ifdef MEM_TRACK_DBG 
#include <stdio.h>
#include <string.h>

void mem_tracker_init(void);
void mem_tracker_cleanup(void);
void print_num_mem_allocated(void);

void *_priv_xmalloc(size_t size, const char *file, int line);
void *_priv_x_aligned_malloc(size_t size, size_t alignment, const char *file, int line);
void *_priv_xcalloc(size_t nmemb, size_t size, const char *file, int line);
void *_priv_xrealloc(void *ptr, size_t size, const char *file, int line);
PVOID _priv_xMmAllocateContiguousMemoryEx(SIZE_T NumberOfBytes, ULONG_PTR Alignment, ULONG Protect, const char *file, int line);
#define xmalloc(size) \
    _priv_xmalloc((size), __FILE__, __LINE__)
#define x_aligned_malloc(size, alignment) \
    _priv_x_aligned_malloc((size), (alignment), __FILE__, __LINE__)
#define xcalloc(nmemb, size) \
    _priv_xcalloc((nmemb), (size), __FILE__, __LINE__)
#define xrealloc(ptr, size) \
    _priv_xrealloc((ptr), (size), __FILE__, __LINE__)
#define xMmAllocateContiguousMemoryEx(NumberOfBytes, Alignment, Protect) \
    _priv_xMmAllocateContiguousMemoryEx((NumberOfBytes), (Alignment), (Protect), __FILE__, __LINE__)

void _priv_xfree(void* ptr, const char *file, int line);
void _priv_x_aligned_free(void* memblock, const char *file, int line);
VOID _priv_xMmFreeContiguousMemory(PVOID BaseAddress, const char *file, int line);
#define xfree(ptr) \
    _priv_xfree((ptr), __FILE__, __LINE__)
#define x_aligned_free(memblock) \
    _priv_x_aligned_free((memblock), __FILE__, __LINE__)
#define xMmFreeContiguousMemory(BaseAddress) \
    _priv_xMmFreeContiguousMemory((BaseAddress), __FILE__, __LINE__)

#else
// Non-debug versions of malloc and free
#define mem_tracker_init(void) do {} while(0)
#define mem_tracker_cleanup(void) do {} while(0)
#define print_num_mem_allocated(void) do {} while(0)
#define xmalloc(size) malloc(size)
#define x_aligned_malloc(size, alignment) _aligned_malloc(size, alignment)
#define xcalloc(nmemb, size) calloc(nmemb, size)
#define xrealloc(ptr, size) realloc(ptr, size)
#define xfree(ptr) free(ptr)
#define x_aligned_free(memblock) _aligned_free(memblock)
#define xMmFreeContiguousMemory(BaseAddress) MmFreeContiguousMemory(BaseAddress)

// #define xMmAllocateContiguousMemoryEx(NumberOfBytes, LowestAcceptableAddress, HighestAcceptableAddress, Alignment, Protect) \
//     MmAllocateContiguousMemoryEx((NumberOfBytes), (LowestAcceptableAddress), (HighestAcceptableAddress), (Alignment), (Protect))
static inline PVOID xMmAllocateContiguousMemoryEx(SIZE_T NumberOfBytes, ULONG_PTR Alignment, ULONG Protect) {
    return MmAllocateContiguousMemoryEx(NumberOfBytes, 0, MAX_MEM_64, Alignment, Protect);
}
#endif
