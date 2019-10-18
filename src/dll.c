#include "usfs/env.h"
#include "usfs/hashtable.h"
#include "usfs/log.h"
#include <dlfcn.h>
#include <fnmatch.h>
#include <stdlib.h>
#include <string.h>

typedef int (*usfs_accept_fn_t)(const char *filename);

#ifndef USFS_ACCEPT_FN_NAME
#define USFS_ACCEPT_FN_NAME "usfs_accept"
#endif

static struct node *nodes[100];
static struct hashtable openlibs = {.size = 100,
                                    .comparator = (compar_t)strcmp,
                                    .hasher = strhash,
                                    .kdtor = free,
                                    .vdtor = (destructor_t)dlclose,
                                    .buckets = nodes};

void *get_symbol(const char *dll_path, const char *sym) {
    void *dlhandle = hashtable_get(&openlibs, dll_path);
    if (!dlhandle) {
        dlhandle = dlopen(dll_path, RTLD_LAZY | RTLD_LOCAL | RTLD_FIRST);
        if (!dlhandle) {
            warn("can't open lib '%s':", dll_path);
            warn("%s\n", dlerror());
            return NULL;
        }
        hashtable_put(&openlibs, strdup(dll_path), dlhandle);
    }
    return dlsym(dlhandle, sym);
}

#ifndef USFS_REREAD_ENV
static struct fs_provider *providers;

const char *find_lib(const char *filename) {
    if (!providers) providers = get_providers();
#else
const char *find_lib(const char *filename) {
    struct fs_provider *providers = get_providers();
#endif  // USFS_REREAD_ENV

    for (int i = 0; providers[i].regex_pattern; i++) {
        if (fnmatch(providers[i].regex_pattern, filename, FNM_FILE_NAME))
            continue;
        usfs_accept_fn_t accept_fn =
            get_symbol(providers[i].dylib_path, USFS_ACCEPT_FN_NAME);
        if (!accept_fn) {
            error(
                "Shared object: '%s' was specified in USFS_PATH but doesn't"
                " contain required symbol '" USFS_ACCEPT_FN_NAME "'\n",
                providers[i].dylib_path);
            return 0;
        }
        if (accept_fn(filename)) return providers[i].dylib_path;
    }
    return 0;
}

#ifdef USFS_CLEANUP
__DTOR static void dll_cleanup() {
    hashtable_destroy(&openlibs);
#ifndef USFS_REREAD_ENV
    if (providers) free(providers)
#endif
}
#endif
