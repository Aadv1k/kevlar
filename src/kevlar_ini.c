#include "kevlar_ini.h"
#include <assert.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"

static ini_table _global_ini_config;

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

static int _h_table_set(ini_table *table, const char *key, const char *value) { return -1; }

static ini_table_node *_h_table_get(ini_table *table, const char *key) { return NULL; }

static void _h_table_destroy(ini_table *table) {
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


int kevlar_ini_table_init(const char *source) {
    _global_ini_config.buckets = INI_TABLE_INIT_SIZE;
    if ((_global_ini_config.nodes = (ini_table_node **)malloc(
             sizeof(ini_table_node *) * _global_ini_config.buckets)) == NULL) {
        return -1;
    }
    _global_ini_config.nodes_count = INI_TABLE_INIT_SIZE;
    return 0;
}

ini_table_node *kevlar_ini_table_get(const char *key) { return NULL; }

void kevlar_ini_table_node_destroy(ini_table_node *node) {
    switch (node->type) {
        case INI_TABLE_NODE_TYPE_FLAT:
            free(node->as.val);
            break;
        case INI_TABLE_NODE_TYPE_NESTED:
            _h_table_destroy(node->as.table);
            break;
    }
    free(node->key);
}

void kevlar_ini_table_destroy() { _h_table_destroy(&_global_ini_config); }
