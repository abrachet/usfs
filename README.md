# USFS User Space File System

USFS uses LD_PRELOAD to override the syscall wrapper symbols in libc and call dynamic libraries specified in USFS_PATH when opening files which match a regular expresison.
