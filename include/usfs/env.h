
#ifndef USFS_ENV_H
#define USFS_ENV_H

struct fs_provider {
    const char *regex_pattern;
    const char *dylib_path;
};

struct fs_provider *get_providers();

#endif  // USFS_ENV_H
