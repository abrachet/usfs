
#include "usfs/hashtable.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct hashtable *hashtable_create(compar_t comp, hasher_t hasher,
                                   destructor_t kdtor, destructor_t vdtor) {
    char *table =
        calloc(1, sizeof(struct hashtable) + sizeof(struct node *) * 100);
    struct hashtable *hashtable = (struct hashtable *)table;
    struct node **buckets = (struct node **)(table + sizeof(struct hashtable));
    hashtable_init(hashtable, buckets, comp, hasher, kdtor, vdtor);
    return hashtable;
}

void hashtable_init(struct hashtable *hashtable, struct node **buckets,
                    compar_t comp, hasher_t hasher, destructor_t kdtor,
                    destructor_t vdtor) {
    hashtable->size = 100;
    hashtable->comparator = comp;
    hashtable->hasher = hasher;
    hashtable->kdtor = kdtor;
    hashtable->vdtor = vdtor;
    hashtable->buckets = buckets;
}

void hashtable_put(struct hashtable *hashtable, void *key, void *value) {
    assert(hashtable);
    size_t bucket = hashtable->hasher(key) % hashtable->size;
    struct node *node = malloc(sizeof(*node));
    assert(node);
    node->entry.key = key;
    node->entry.value = value;
    node->next = hashtable->buckets[bucket];
    hashtable->buckets[bucket] = node;
}

struct node_loc {
    struct node *prev, *found;
    size_t index;
};

static struct node_loc find_node(struct hashtable *hashtable, const void *key) {
    struct node_loc ret;
    ret.prev = 0;
    ret.index = hashtable->hasher(key) % hashtable->size;
    for (ret.found = hashtable->buckets[ret.index];
         ret.found && hashtable->comparator(ret.found->entry.key, key);
         ret.prev = ret.found, ret.found = ret.found->next)
        ;
    return ret;
}

void *hashtable_get(struct hashtable *hashtable, const void *key) {
    assert(hashtable);
    struct node_loc loc = find_node(hashtable, key);
    return loc.found ? loc.found->entry.value : 0;
}

void hashtable_remove(struct hashtable *hashtable, const void *key) {
    assert(hashtable);
    struct node_loc loc = find_node(hashtable, key);
    if (!loc.found) return;
    if (loc.prev)
        loc.prev->next = loc.found->next;
    else
        hashtable->buckets[loc.index] = loc.found->next;

    hashtable->kdtor(loc.found->entry.key);
    hashtable->vdtor(loc.found->entry.value);
    free(loc.found);
}

static void free_list(struct node *head, destructor_t kdtor,
                      destructor_t vdtor) {
    if (!head) return;

    free_list(head->next, kdtor, vdtor);
    kdtor(head->entry.key);
    vdtor(head->entry.value);
    free(head);
}

void hashtable_destroy(struct hashtable *hashtable) {
    assert(hashtable);
    for (int i = 0; i < hashtable->size; i++) {
        if (hashtable->buckets[i])
            free_list(hashtable->buckets[i], hashtable->kdtor,
                      hashtable->vdtor);
    }
}
