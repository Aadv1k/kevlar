#include "kevlar_ini.h"
#include "kevlar_errors.h"
#include "utils.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"

#define IS_DELIM(c) ((c) == '=' || (c) == ':')
#define IS_COMMENT(c) ((c) == ';' || (c) == '#')
#define WITHIN_BOUNDS(i, b) ((i) <= (b))

#define ini_parser_panic(m, c, l) kevlar_err("Bad .ini syntax at line %zu, col %zu: %s", l, c, m)


static ini_table *_global_ini_config;
static char* _current_section_label;

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
        node->next = NULL;

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
            assert(cur_node->val != NULL);

            free(cur_node->val);
            cur_node->val = strdup(value);

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
    node->next = NULL;
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

        while (cur) {
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
    if ((table->nodes = (ini_table_node **)calloc(table->buckets, sizeof(ini_table_node *))) ==
        NULL) {
        kevlar_err("Out of memory: failed to allocate hash table node array");
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
    if (_current_section_label != NULL) free(_current_section_label);
    _current_section_label = NULL;

    free(_global_ini_config);
}

char* _kevlar_ini_parse_val(const char* src, size_t len, size_t* cur, size_t* lnum, ini_parser_error* error) {
    for (size_t i = *cur; i <= len; ++i) {

        if (src[i] == '\n' || (i+1 > len) || (IS_COMMENT(src[i]) && i+1 <= len && src[i+1] == ' ')) {
            (*lnum)++;


            if (WITHIN_BOUNDS(i+1, len) && src[i+1] == '\t') {
                (void)i++;
                continue;
            }

            size_t val_size = i - *cur;

            if (val_size == 0) {
                error->line = *lnum;
                error->col= i;
                error->code = INI_ERR_INVALID_SYNTAX;
                snprintf(error->message, INI_ERR_MSG_SIZE, "Key has no value");
                return NULL;
            }

            assert(*cur < i);

            char* val_buffer = (char*)malloc(val_size + 1);
            if (!val_buffer) {
                error->line = *lnum;
                error->col = i;
                error->code = INI_ERR_OOM;
                snprintf(error->message, INI_ERR_MSG_SIZE, "Out of memory: failed to allocate %zu bytes for value", val_size + 1);
                return NULL;
            }

            strncpy(val_buffer, &src[*cur], val_size);
            val_buffer[val_size] = '\0';

            *cur = i + 1;

            const char* pos;
            size_t val_size_cpy = val_size;

            size_t offset = 1;

            while ((pos = strstr(val_buffer, "\n\t"))) {
                val_size_cpy -= 1;
                size_t tail = (pos - val_buffer);
                memmove(&val_buffer[tail], &val_buffer[tail+offset], val_size_cpy - tail);
                val_buffer[tail] = ' ';
            }
            val_buffer[val_size_cpy] = '\0';

            return val_buffer;
        }
    }

    return NULL;
}


static int _kevlar_parse_section_label(const char* src, size_t len, size_t* cur, size_t* lnum, ini_parser_error* error) {
    size_t start = *cur;

    while (*cur < len && src[*cur] != ']' && src[*cur] != '[' && src[*cur] != '\n') {
        (*cur)++;
    }

    if (*cur >= len || src[*cur] != ']') {
        error->line = *lnum;
        error->col = *cur;
        error->code = INI_ERR_INVALID_LABEL;
        snprintf(error->message, INI_ERR_MSG_SIZE, "Malformed section label, missing closing ']'");
        return -1;
    }

    size_t end = *cur;
    (*cur)++;

    size_t content_len = end - start;

    int all_spaces = 1;
    for (size_t i = start; i < end; i++) {
        if (src[i] != ' ' && src[i] != '\t') {
            all_spaces = 0;
            break;
        }
    }

    if (content_len == 0 || all_spaces) {
        error->line = *lnum;
        error->col = *cur;
        error->code = INI_ERR_INVALID_LABEL;
        snprintf(error->message, INI_ERR_MSG_SIZE, "Section label cannot be empty or whitespace-only");
        return -1;
    }

    if (_current_section_label != NULL) {
        free(_current_section_label);
    }

    _current_section_label = malloc(content_len + 1);
    if (!_current_section_label) {
        error->line = *lnum;
        error->col = *cur;
        error->code = INI_ERR_OOM;
        snprintf(error->message, INI_ERR_MSG_SIZE, "Out of memory: failed to allocate %zu bytes for section label", content_len + 1);
        return -1;
    }

    memcpy(_current_section_label, src + start, content_len);
    _current_section_label[content_len] = '\0';

    return 0;
}

int _kevlar_ini_parse(const char* src, size_t len, size_t* cur, size_t* lnum, ini_parser_error* error) {
    for (size_t i = *cur; i <= len; ++i) {

        if (src[i] == '[') {
            i++;
            if (_kevlar_parse_section_label(src, len, &i, lnum, error) == -1) {
                return -1;
            }
            *cur = i + 1;
        }

        if (src[i] == ']') {
            error->line = *lnum;
            error->col = i;
            error->code = INI_ERR_INVALID_SYNTAX;
            snprintf(error->message, INI_ERR_MSG_SIZE, "Unexpected ']' without opening '['");

            return -1;
        }

        if (IS_COMMENT(src[i])) {
            char *new_l = strchr(&src[i], '\n');

            // Single line in the entire sourcd which is a comment, functionally empty
            if (!new_l) return 0;

            *cur = (new_l - &src[i]) + 1;
            continue;
        }

        if (IS_DELIM(src[i])) {
            if (i <= *cur) {
                error->line = *lnum;
                error->col = i;
                error->code = INI_ERR_INVALID_KEY;
                snprintf(error->message, INI_ERR_MSG_SIZE, "Expected a valid key preceding the delimiter");
                return -1;
            }

            assert(*cur < i);

            size_t k_size = i - *cur;
            size_t section_label_size = (_current_section_label != NULL) ? strlen(_current_section_label) : 0;
            size_t dot = (_current_section_label != NULL) ? 1 : 0;
            size_t total_key_size = section_label_size + dot + k_size + 1;
            char* key_buffer = (char*)malloc(total_key_size);
            if (!key_buffer) {
                error->line = *lnum;
                error->col = i;
                error->code = INI_ERR_OOM;
                snprintf(error->message, INI_ERR_MSG_SIZE, "Out of memory: failed to allocate %zu bytes for key", total_key_size);
                return -1;
            }

            if (_current_section_label != NULL) {
                snprintf(key_buffer, section_label_size + dot + k_size + 1, "%s.%.*s", _current_section_label, (int)k_size, &src[*cur]);
            } else {
                snprintf(key_buffer, k_size + 1, "%.*s", (int)k_size, &src[*cur]);
            }
            utl_strip(key_buffer);

            i++;
            char* value = _kevlar_ini_parse_val(src, len, &i, lnum, error);
            if (value == NULL) {
                free(key_buffer);
                return -1;
            }
            utl_strip(value);
            i--;

            if (kevlar_ini_table_set(key_buffer, value) == -1) {
                ini_parser_panic("Failed to insert key-value pair into table", i, *lnum);
            }

            free(key_buffer);
            free(value);

            *cur = i + 1;
        }
    }

    return 0;
}

int kevlar_ini_table_init() {
    ini_table *table;
    if ((table = _h_table_init()) == NULL) {
        return -1;
    }
    _global_ini_config = table;


    return 0;
}

int kevlar_ini_parse(const char* src, ini_parser_error* error) {
    size_t src_len = strlen(src);
    size_t cursor = 0,
            lnum = 1;

    return _kevlar_ini_parse(src, src_len, &cursor, &lnum, error);
}
