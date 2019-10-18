
#include <stdatomic.h>

#define UNKNOWN_OWNER ((char *)0)
#define OS_OWNED ((char *)-1)

struct filedes {
    atomic_uint ref_count;
    int flags;          // flags it was open(2)ed with
    char *path_to_lib;  // 0 if this file is managed by the system
};

int register_fd(int fd, int flags, char *lib);
int add_ref(int fd);
int remove_ref(int fd);
struct filedes *get_fd(int fd);
