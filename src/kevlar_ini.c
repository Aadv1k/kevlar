#include "kevlar_ini.h"
#include <assert.h>
#include <string.h>
#include <stdint.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"

static ini_table* _global_ini_config;

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

int _h_table_set_str(ini_table *table, const char *key, const char *value) {
    uint64_t key_hash = fnv1_hash(key),
            key_node_idx = key_hash % table->buckets;

    if (table->nodes[key_node_idx] == NULL) {
        ini_table_node* node = malloc(sizeof(ini_table_node));
        node->key = strdup(key);
        node->val = strdup(value);

        table->nodes[key_node_idx] = node;
        table->nodes_count++;
        return 0;
    }

    ini_table_node* cur_node = table->nodes[key_node_idx];
    ini_table_node* last_tail_node = NULL;

    // Exhaust all cases where the key is already present within the linked
    // list, i.e we are overriding an existing key rather than setting
    // completely new one
    while (cur_node != NULL) {
        if (strcmp(cur_node->key, key) == 0) {
            ini_table_node* node = table->nodes[key_node_idx];

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

    // TODO: malloc and return


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

ini_table* _h_table_init() {
    ini_table* table = malloc(sizeof(ini_table));

    table->buckets = INI_TABLE_INIT_SIZE;
        if ((table->nodes = (ini_table_node **)malloc(
                 sizeof(ini_table_node *) * table->buckets)) == NULL) {
            return NULL;
        }
        table->nodes_count = 0;
        return table;
}

int kevlar_ini_table_init(const char *source) {
    ini_table* table;
    if ((table = _h_table_init()) == NULL)  {
        return -1;
    }
    _global_ini_config = table;
    return 0;
}

ini_table_node *kevlar_ini_table_get(const char *key) { return NULL; }

void kevlar_ini_table_node_destroy(ini_table_node *node) {
    free(node->val);
    free(node->key);
}

void kevlar_ini_table_destroy() {
    _h_table_destroy(_global_ini_config);
    free(_global_ini_config);
}
