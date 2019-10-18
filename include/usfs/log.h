
#ifndef USFS_LOG_H
#define USFS_LOG_H

#include <stdio.h>

extern FILE *usfs_log;

int error(const char *restrict fmt, ...);
int warn(const char *restrict fmt, ...);

#endif  // USFS_LOG_H
