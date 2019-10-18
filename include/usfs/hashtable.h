
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef int (*compar_t)(const void *, const void *);
typedef size_t (*hasher_t)(const void *);
typedef void (*destructor_t)(void *);

struct entry {
    void *key;
    void *value;
};

struct node {
    struct entry entry;
    struct node *next;
};

// Provide the definition in the header file so a hashtable can be made
// not just from hashtable_create.
struct hashtable {
    size_t size;
    compar_t comparator;
    hasher_t hasher;
    destructor_t kdtor;
    destructor_t vdtor;
    struct node **buckets;
};

static size_t strhash(const void *str) {
    const char *string = str;
    unsigned long hash = 5381;
    for (int c; (c = *string++);) hash = ((hash << 5) + hash) + c;
    return hash;
}

static void null_dtor(void *_) {
    (void)_;
    return;
}

struct hashtable *hashtable_create(compar_t, hasher_t, destructor_t kdtor,
                                   destructor_t vdtor);
void hashtable_init(struct hashtable *, struct node **buckets, compar_t,
                    hasher_t, destructor_t kdtor, destructor_t vdtor);

static inline struct hashtable *create_str_hashtable(destructor_t value_dtor) {
    return hashtable_create((compar_t)strcmp, strhash, free, value_dtor);
}

void hashtable_put(struct hashtable *hashtable, void *key, void *value);
void *hashtable_get(struct hashtable *hashtable, const void *key);
void hashtable_remove(struct hashtable *hashtable, const void *key);
void hashtable_destroy(struct hashtable *hashtable);
