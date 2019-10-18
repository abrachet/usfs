
#include "usfs/fdtable.h"
#include "usfs/log.h"
#include "usfs/usfs.h"
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>

// TODO: size will not be shared so it will be unsafe to grow the list.
struct vector {
    size_t size;
    struct filedes *files;
};

static struct vector fd_vec;

__CTOR static void init_fd_vec() {
    struct rlimit max_fd;
    getrlimit(RLIMIT_NOFILE, &max_fd);
    fd_vec.files = mmap(0, max_fd.rlim_cur, PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANON, 0, 0);
    if (fd_vec.files == MAP_FAILED) {
        fd_vec.files = 0;
        error("mmap failed\n");
    }
    fd_vec.size = max_fd.rlim_cur;
}

static int grow_vec() { return -EMFILE; }

static inline int insert_fd(int fd, int flags, char *lib) {
    int err;
    if (fd >= fd_vec.size && (err = grow_vec()))
        return err;
    if (atomic_load(&fd_vec.files[fd].ref_count)) {
        error("fd %d is already being referenced\n", fd);
        return -EBADF;
    }
    atomic_fetch_add(&fd_vec.files[fd].ref_count, 1);
    fd_vec.files[fd].flags = flags;
    fd_vec.files[fd].path_to_lib = strdup(lib);
    return 0;
}

int register_fd(int fd, int flags, char *lib) {
    if (fd < fd_vec.size && atomic_load(&fd_vec.files[fd].ref_count)) {
        error("fd %d still has references but it is being overwritten\n", fd);
        return -EBADF;
    }
    int ret = insert_fd(fd, flags, (lib != OS_OWNED) ? strdup(lib) : lib);
    return ret;
}

int add_ref(int fd) {
    if (fd >= fd_vec.size) {
        error("adding reference to unkown file descriptor\n");
        return -EBADF;
    }
    if (!atomic_load(&fd_vec.files[fd].ref_count)) {
        error("cannot add reference to fd with unkown status\n");
        return -EBADF;
    }

    atomic_fetch_add(&fd_vec.files[fd].ref_count, 1);
    return 0;
}

int remove_ref(int fd) {
    if (fd >= fd_vec.size) {
        error("removing reference to unkown file descriptor\n");
        return -EBADF;
    }
    assert(atomic_load(&fd_vec.files[fd].ref_count));

    if (atomic_fetch_sub(&fd_vec.files[fd].ref_count, 1) == 1) {
        char *path = fd_vec.files[fd].path_to_lib;
        if (path)
            free(path);
        fd_vec.files[fd].path_to_lib = 0;
        fd_vec.files[fd].flags = 0;
    }

    return 0;
}

struct filedes *get_fd(int fd) {
    return fd >= fd_vec.size ? 0 : fd_vec.files + fd;
}
