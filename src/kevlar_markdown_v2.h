#ifndef KEVLAR_MARKDOWN_V2_H_
#define KEVLAR_MARKDOWN_V2_H_

#include <stddef.h>

#define ARENA_ALIGNMENT_CONST sizeof(void*)

#define ALIGN_POW2(cap, ac) ((uint64_t)(cap) + (uint64_t)(ac) - 1) & ~((uint64_t)(ac) - 1)

#define MiB(v) ((v) << 20)
#define KiB(v) ((v) << 10)

typedef struct mem_arena {
    size_t capacity;
    size_t pos;
    void* bytes;
} mem_arena;

mem_arena* arena_create(size_t capacity);
void* arena_allocate(mem_arena* arena, size_t size);
void arena_reset(mem_arena* arena);
void arena_destroy(mem_arena* arena);

#endif // KEVLAR_MARKDOWN_V2_H_
