#include "kevlar_ini.h"
#include "kevlar_errors.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"

static ini_table *_global_ini_config;

// Source: https://www.ietf.org/archive/id/draft-eastlake-fnv-22.html
uint64_t fnv1_hash(const char *input) {
    const char *p = input;

    uint64_t hash = FNV_OFFSET;

    while (*p != '\0') {
        hash ^= *p;
        hash *= FNV_PRIME;
        p++;
    }

    return hash;
}

const char *_h_table_get(ini_table *table, const char *key) {
    uint64_t key_hash = fnv1_hash(key), key_node_idx = key_hash % table->buckets;

    ini_table_node *head = table->nodes[key_node_idx];
    if (head == NULL)
        return NULL;
    if (strcmp(head->key, key) == 0)
        return head->val;

    while (head->next) {
        if (strcmp(head->next->key, key) == 0)
            return head->next->val;
        head = head->next;
    }

    return NULL;
}

int _h_table_set_str(ini_table *table, const char *key, const char *value) {
    uint64_t key_hash = fnv1_hash(key), key_node_idx = key_hash % table->buckets;

    if (table->nodes[key_node_idx] == NULL) {
        ini_table_node *node = malloc(sizeof(ini_table_node));
        node->key = strdup(key);
        node->val = strdup(value);

        table->nodes[key_node_idx] = node;
        table->nodes_count++;
        return 0;
    }

    ini_table_node *cur_node = table->nodes[key_node_idx];
    ini_table_node *last_tail_node = NULL;

    // Exhaust all cases where the key is already present within the linked
    // list, i.e we are overriding an existing key rather than setting
    // completely new one
    while (cur_node != NULL) {
        if (strcmp(cur_node->key, key) == 0) {
            ini_table_node *node = table->nodes[key_node_idx];

            assert(node->val != NULL);

            free(node->val);
            node->val = strdup(value);

            return 0;
        }

        last_tail_node = cur_node;
        cur_node = cur_node->next;
    }

    // Not possible to reach here unless some node's neighbour was  null
    assert(last_tail_node != NULL);

    ini_table_node *node = malloc(sizeof(ini_table_node));

    node->key = strdup(key);
    node->val = strdup(value);
    last_tail_node->next = node;

    return 0;
}

void _h_table_destroy(ini_table *table) {
    assert(table != NULL);

    for (size_t i = 0; i < table->buckets; ++i) {
        ini_table_node *cur = table->nodes[i];
        if (cur == NULL)
            continue;

        ini_table_node *next;

        for (;;) {
            if (cur->next == NULL) {
                kevlar_ini_table_node_destroy(cur);
                free(cur);
                break;
            }
            next = cur->next;
            kevlar_ini_table_node_destroy(cur);
            free(cur);

            cur = next;
        }
    }
    free(table->nodes);
}

ini_table *_h_table_init() {
    ini_table *table = malloc(sizeof(ini_table));

    table->buckets = INI_TABLE_INIT_SIZE;
    if ((table->nodes = (ini_table_node **)malloc(sizeof(ini_table_node *) * table->buckets)) ==
        NULL) {
        return NULL;
    }
    table->nodes_count = 0;
    return table;
}


const char* kevlar_ini_table_get(const char *key) {
    assert(_global_ini_config != NULL && "global ini config must be initialized");
    return _h_table_get(_global_ini_config, key);
}

int kevlar_ini_table_set(const char *key, const char *value) {
    assert(_global_ini_config != NULL && "global ini config must be initialized");
    return _h_table_set_str(_global_ini_config, key, value);
}

void kevlar_ini_table_node_destroy(ini_table_node *node) {
    free(node->val);
    free(node->key);
}

void kevlar_ini_table_destroy() {
    _h_table_destroy(_global_ini_config);
    free(_global_ini_config);
}

#define IS_DELIM(c) ((c) == '=' || (c) == ':')
#define WITHIN_BOUNDS(i, b) ((i) <= (b))

#define ini_parser_panic(m, c, l) kevlar_err("Bad .ini syntax at line %zu, col %zu: %s", l, c, m)

char* _kevlar_ini_parse_val(const char* src, size_t len, size_t* cur, size_t* lnum) {
    for (size_t i = *cur; i <= len; ++i) {
        if (src[i] == '\n' || i+1 > len) {
            (void)*lnum++;

            if (WITHIN_BOUNDS(i+1, len) && src[i+1] == '\t') {
                (void)i++;
                continue;
            }

            assert(*cur < i);
            size_t val_size = i - *cur + 1;
            char* val_buffer = (char*)malloc(val_size*sizeof(char));
            strncpy(val_buffer, &src[*cur], i-1);

            *cur = i;
            return val_buffer;
        }
    }

    return NULL;
}

void _kevlar_ini_parse(const char* src, size_t len, size_t* cur, size_t* lnum, const char* section_label) {
    for (size_t i = *cur; i <= len; ++i) {
        if (IS_DELIM(src[i])) {
            if (i - 1 <= *cur)
                ini_parser_panic("Expected a valid key preceeding a delimeter.", i, lnum);

            assert(*cur < i);

            size_t k_size = i - *cur + 1;
            char* key_buffer = (char*)malloc(sizeof(k_size));
            strncpy(key_buffer, &src[*cur], i);

            i++;
            char* value = _kevlar_ini_parse_val(src, len, &i, lnum);
            if (value == NULL) {
                ini_parser_panic("Could not parse value", i, *lnum);
            }

            if (kevlar_ini_table_set(key_buffer, value) == -1) {
                ini_parser_panic("Failed to set value to map", i, *lnum);
            }

            free(key_buffer);
            free(value);
        }
    }
}

int kevlar_ini_table_init(const char *source) {
    ini_table *table;
    if ((table = _h_table_init()) == NULL) {
        return -1;
    }
    _global_ini_config = table;


    size_t src_len = strlen(source);
    size_t cursor = 0,
            lnum = 1;

    _kevlar_ini_parse(source, src_len, &cursor, &lnum, "");

    return 0;
}
