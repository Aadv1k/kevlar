#ifndef kevlar_ini_h_
#define kevlar_ini_h_

#include <stdlib.h>
#include <stdint.h>

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#define INI_ERR_MSG_SIZE 256

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

typedef enum {
    INI_OK = 0,
    INI_ERR_INVALID_SYNTAX,
    INI_ERR_INVALID_KEY,
    INI_ERR_INVALID_LABEL,
    INI_ERR_OOM,
} ini_parse_code;


typedef struct {
    size_t line;
    size_t col;
    ini_parse_code code;
    char message[INI_ERR_MSG_SIZE];
} ini_parser_error;

uint64_t fnv1_hash(const char* input);

int _h_table_set_str(ini_table *table, const char *key, const char *value);
const char* _h_table_get(ini_table *table, const char *key);
void _h_table_destroy(ini_table *table);
ini_table* _h_table_init();

int kevlar_ini_table_init();
const char* kevlar_ini_table_get(const char* key);
int kevlar_ini_table_set(const char *key, const char *value);
int kevlar_ini_parse(const char* src, ini_parser_error* error);
void kevlar_ini_table_node_destroy(ini_table_node *node);
void kevlar_ini_table_destroy();

#endif
