
#ifndef USFS_DLL_H
#define USFS_DLL_H

void *get_symbol(const char *dll_path, const char *sym);
const char *find_lib(const char *filename);

#endif  // USFS_DLL_H
