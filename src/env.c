#include "usfs/env.h"
#include "usfs/log.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline size_t count_occur(const char *s, char c) {
    size_t num = 0;
    for (; (s = strchr(s, c) + 1) != (const char *)1; num++)
        ;
    return num;
}

static inline const char *strchr_orend(const char *s, int c) {
    const char *ret = strchr(s, c);
    return ret ? ret : s + strlen(s);
}

struct fs_provider *get_providers() {
    const char *usfs_path = getenv("USFS_PATH");
    size_t num_provider = count_occur(usfs_path, ':');
    if (!(num_provider % 2)) {
        error("Wrong number of arguments for USFS_PATH variable\n");
        return 0;
    }

    num_provider = (num_provider + 1) / 2;
    struct fs_provider *providers =
        malloc(sizeof(*providers) * (num_provider + 1));
    assert(providers);
    providers[num_provider] = (struct fs_provider){0};

    for (ssize_t i = 0; i < num_provider; i++) {
        const char *end = strchr(usfs_path, ':') - 1;
        providers[i].regex_pattern = strndup(usfs_path, end - usfs_path + 1);
        usfs_path = end + 2;
        end = strchr_orend(usfs_path, ':');
        providers[i].dylib_path = strndup(usfs_path, end - usfs_path);
        usfs_path = end + 1;
    }

    return providers;
}
