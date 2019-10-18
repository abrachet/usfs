
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int usfs_accept(const char *name) {
    return 1;
}

int usfs_open(const char *name, int flags) {
    return dup(0);
}

ssize_t usfs_read(int fd, void *buf, size_t nbyte) {
    puts("got called");
    strcpy(buf, "/dev/zero");
    return 10;
}
