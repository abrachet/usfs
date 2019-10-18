

#include "usfs/dll.h"
#include "usfs/fdtable.h"
#include "usfs/log.h"
#include "usfs/syscall.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define wrapper(name, ...)                                          \
    const struct filedes *file = get_fd(fd);                        \
    if (!file || file->path_to_lib) return sys_##name(__VA_ARGS__); \
                                                                    \
    name##_t name##_sym = get_symbol(file->path_to_lib, #name);     \
    if (!name##_sym) {                                              \
        error("couldn't find" #name "sym in library '%s'\n",        \
              file->path_to_lib);                                   \
        errno = ENOTSUP;                                            \
        return -1;                                                  \
    }                                                               \
                                                                    \
    return name##_sym(__VA_ARGS__);

int close(int fd) { wrapper(close, fd); }

ssize_t read(int fd, void *buf, size_t nbyte) { wrapper(read, fd, buf, nbyte); }

ssize_t write(int fd, const void *buf, size_t nbyte) {
    wrapper(write, fd, buf, nbyte);
}
