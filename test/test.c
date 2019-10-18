#include "test.h"
#include "usfs/env.h"
#include "usfs/hashtable.h"
#include <stdlib.h>
#include <string.h>

void env_test() {
    setenv("USFS_PATH", "regex:dylib_path", 1);
    struct fs_provider *providers = get_providers();

    ASSERT(!providers[1].regex_pattern);
    ASSERT(!providers[1].dylib_path);
    ASSERT(!strcmp(providers[0].regex_pattern, "regex"));
    ASSERT(!strcmp(providers[0].dylib_path, "dylib_path"));

    setenv("USFS_PATH", "regex:dylib_path:/dev/*:usdev", 1);
    providers = get_providers();
    ASSERT(!providers[2].regex_pattern);
    ASSERT(!providers[2].dylib_path);
    ASSERT(!strcmp(providers[0].regex_pattern, "regex"));
    ASSERT(!strcmp(providers[0].dylib_path, "dylib_path"));
    ASSERT(!strcmp(providers[1].regex_pattern, "/dev/*"));
    ASSERT(!strcmp(providers[1].dylib_path, "usdev"));
}

void str_hashtable_test() {
    struct hashtable *table = create_str_hashtable(null_dtor);
    hashtable_put(table, strdup("one"), (void*)1);
    ASSERT((int) hashtable_get(table, "one") == 1);
    hashtable_put(table, strdup("one"), (void*)2);
    ASSERT((int) hashtable_get(table, "one") == 2);
    hashtable_remove(table, "one");
    ASSERT((int) hashtable_get(table, "one") == 1);
    hashtable_remove(table, "one");
    ASSERT(hashtable_get(table, "one") == NULL);
}

int main() {
    env_test();
    str_hashtable_test();
}
