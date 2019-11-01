# USFS User Space File System

USFS uses LD_PRELOAD to override the syscall wrapper symbols in libc and call dynamic libraries specified in USFS_PATH when opening files which match a regular expression.

## Installation
```bash
git clone https://github.com/abrachet/usfs.git
cd usfs
./configure --libdir=<dir> # see ./configure --help for more options
make
make install
```

## Usage
Libraries which wish to make use of USFS must provide the symbol `usfs_accept` which takes a `const char *` and returns a nonzero integer if the given filename should be associated with that library. First before a libraries `usfs_accept` can be called it must first be found by USFS as a candidate for a given file. This is done using the `USFS_PATH` environment variable, entries are separated by a single ':' character, an entry is in the form `<regex>:<path_to_lib>`. When any syscall associated with a file name is called, USFS will search through `USFS_PATH` to find a matching regex pattern, on match USFS will load that library and call its `usfs_accept` to see if all subsequent syscalls associated with that file should be handled by that library. Once a file gets associated with a library all related syscalls will attempt to redirect to that library. This association is usually with a file descriptor returned from `open`. Any syscalls on a file descriptor opened this way will first search the libraries `usfs_<syscall>`, if that symbol is not found USFS will fall back to calling the actual syscall. Often a library will not want to provide definitions for every file related syscall, and a clever `usfs_open` implementation returning a file descriptor to a valid file makes this easy. To ensure any file descriptors returned from a library syscall do not interfere with system file descriptors, libraries are required to call upon the OS to actually give them a new file descriptor in any way that the library sees fit. 

## Rationale
USFS is very similar to FUSE (Filesystem in Userspace), both have strength and weaknesses. Foremost, FUSE is much more robust, a FUSE filesystem will work exactly as any other filesystem for any user program which uses it. This cannot be enforced with USFS because USFS only overrides weak libc symbols and provide its own definition. This usually works because most programs especially those which wish to be portable rely on libc to make syscalls. The biggest point of pain for USFS is abstracting directory structure, there is no portable syscall to for which `readdir(3)` always calls, so USFS must override `readdir(3)` and other section 3 library functions in `dirent.h` which is more prone to error. USFS can in some situations do more than FUSE, for example, a FUSE filesystem must be mounted in a unique directory, it would not be possible for a library using fuse to provide its own `/dev/` files for example. This is possible with USFS. Moreover, FUSE requires that libraries to abstract files be linked against libfuse, but USFS makes no such requirement, and is overall much easier to work with. USFS exists solely in userspace, it pays a heavy price of robustness for this, unfortunately. USFS is a project for fun, it will never and should never be used in a production environment like how FUSE can.

# Future Direction
Very few syscall wrappers are actually implemented here, only the basic ones used for primary testing. Future syscall wrappers must first be added to utils/syscalls.py before they can be implemented in either src/fd_syscalls.c or src/filename_syscalls.c. Adding syscalls is trivial for most of them, especially fd_syscalls which can usually be done using the macro `wrapper`.
