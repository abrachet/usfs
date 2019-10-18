

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    assert(!strcmp("/dev/zero:dev_zero.so", getenv("USFS_PATH")));
    int fd = open("/dev/zero", O_RDONLY);
    char buf[10] = {0};
    read(fd, buf, 10);
    puts(buf);
    return !!strcmp(buf, "/dev/zero");
}
