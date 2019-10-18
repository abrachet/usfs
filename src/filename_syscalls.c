
#include "usfs/fdtable.h"
#include "usfs/syscall.h"
#include "usfs/dll.h"
#include "usfs/log.h"
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

struct sym_and_path {
    void *sym;
    char *dylib_path;
};

static struct sym_and_path try_entry_point(const char *filename, const char *sym) {
    const char *lib_path = find_lib(filename);
    if (!lib_path) return (struct sym_and_path){0};
    void *found_sym = get_symbol(lib_path, sym);
    if (!found_sym) {
        warn("Shared object: '%s' doesn't export function '%s'\n", lib_path,
             sym);
        return (struct sym_and_path){0};
    }
    return (struct sym_and_path){.sym = found_sym,
                                 .dylib_path = strdup(lib_path)};
}

int open(const char *pathname, int oflag, ...) {
    va_list ap;
    va_start(ap, oflag);
    mode_t mode = va_arg(ap, int);
    va_end(ap);
    struct sym_and_path sym = try_entry_point(pathname, "usfs_open");
    int fd;
    if (sym.sym) {
        fd = ((open_t)sym.sym)(pathname, oflag, mode);
        register_fd(fd, oflag, sym.dylib_path);
        free(sym.dylib_path);
    } else {
        fd = sys_open(pathname, oflag, mode);
        register_fd(fd, oflag, OS_OWNED);
    }
    return fd;
}

int creat(const char *pathname, mode_t mode) {
    return open(pathname, O_CREAT | O_WRONLY | O_TRUNC, mode);
}

int access(const char *pathname, int mode) {
    struct sym_and_path sym = try_entry_point(pathname, "access");
    int ret;
    if (sym.sym) {
        ret = ((access_t)sym.sym)(pathname, mode);
        free(sym.dylib_path);
    } else {
        ret = sys_access(pathname, mode);
    }
    return ret;
}
