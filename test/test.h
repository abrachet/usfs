
#ifndef USFS_TEST_H
#define USFS_TEST_H

#include <stdio.h>


#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define LINE_STRING STRINGIZE(__LINE__)

#define FAILURE_MSG "\033[31m[ FAILED ]\033[0m"__FILE__ ":" \
                    LINE_STRING " assertion failed: "

#ifndef QUIET
#define SUCCESS_MSG "\033[32m[   OK   ]\033[0m\n"
#else
#define SUCCESS_MSG
#endif

#define ASSERT(cond)                    \
{                                       \
    if (!(cond)) {                      \
        fputs(FAILURE_MSG #cond "\n", stderr); \
        return;                         \
    } else {                            \
        fputs(SUCCESS_MSG, stderr);     \
    }                                   \
}

typedef void (*test_t)(void);

void register_test(test_t test);

#endif
