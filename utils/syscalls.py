
syscalls = {
    "read": ["ssize_t", "int", "char *", "size_t"],
    "write": ["ssize_t", "int", "const char *", "size_t"],
    "open": ["int", "const char *", "int", "mode_t"],
    "close": ["int", "int"],
    "access": ["int", "const char *", "int"],
    "link": ["int", "const char *", "const char *"],
    "unlink": ["int", "const char *"],
    "execve": ["int", "const char *", "const char *[]", "const char *[]"],
    "chdir": ["int", "const char *"]
}
