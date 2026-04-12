#ifndef kevlar_ini_h_
#define kevlar_ini_h_

#include <stdlib.h>
#include <stdint.h>

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#define INI_TABLE_INIT_SIZE 67

typedef struct ini_table ini_table;

typedef struct ini_table_node {
    char* key;
    char* val;

    struct ini_table_node* next;
} ini_table_node;

struct ini_table {
    size_t buckets;
    size_t nodes_count;
    ini_table_node** nodes;
};

uint64_t fnv1_hash(const char* input);

int _h_table_set_str(ini_table *table, const char *key, const char *value);
void _h_table_destroy(ini_table *table);
ini_table* _h_table_init();

int kevlar_ini_table_init(const char* source);
ini_table_node* kevlar_ini_table_get(const char* key);
void kevlar_ini_table_node_destroy(ini_table_node *node);
void kevlar_ini_table_destroy();

#endif
