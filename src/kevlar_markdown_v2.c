#include "./kevlar_markdown_v2.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

mem_arena* arena_create(size_t capacity) {
    mem_arena* arena = (mem_arena*)malloc(sizeof(mem_arena));

    arena->capacity = capacity;
    arena->pos = 0;
    if ((arena->bytes = malloc(arena->capacity)) == NULL) return NULL;

    return arena;
}

void* arena_allocate(mem_arena* arena, size_t size) {
    size_t aligned_size = ALIGN_POW2(size, ARENA_ALIGNMENT_CONST);
    
    if ((arena->capacity - arena->pos) < aligned_size) {
        return NULL;
    }

    void* p = (char*)arena->bytes + arena->pos;
    arena->pos += aligned_size;

    return p;
}

void arena_destroy(mem_arena* arena) {
    free(arena->bytes);
    free(arena);
}
