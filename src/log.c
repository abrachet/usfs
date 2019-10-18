#define _GNU_SOURCE
#include "usfs/log.h"
#include "usfs/usfs.h"
#include <assert.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Cannot freopen stderr and interfere with the running programs stderr.
FILE *usfs_log;

enum etype { None, Die, Warn, Log, Debug };

static enum etype error_severity =
#ifdef USFS_DEBUG
    Debug;
#else
    None;
#endif

__CTOR static void find_error_action() {
    const char *action = getenv("USFS_ERROR");
    if (!action)
        return;

    if (!strcasecmp(action, "die"))
        error_severity = Die;
    else if (!strcasecmp(action, "warn"))
        error_severity = Warn;
}

__CTOR static void open_streams() {
    const char *log_dir = getenv("USFS_LOG_DIR");
    if (error_severity == Debug) {
        usfs_log = stderr;
        return;
    }

    char *stderr_path = "/dev/null";

    time_t rawtm;
    time(&rawtm);
    struct tm *tm = gmtime(&rawtm);

    char pid_buf[11];
    char *progname = getenv("_");
    if (!progname) {
        sprintf(pid_buf, "%d", getpid());
        progname = pid_buf;
    } else {
        progname = strrchr(progname, '/') + 1;
    }

    if (log_dir)
        asprintf(&stderr_path, "%s/usfs_%s_%02d%02d-%d.%d.%d.log", log_dir,
                 progname, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min,
                 tm->tm_sec);

    usfs_log = fopen(stderr_path, "a");
    if (log_dir)
        free(stderr_path);
}

static int _log(enum etype severity, const char *restrict fmt, va_list ap) {
    if (severity == None)
        return 0;

    if (severity == Die) {
        int ret = fprintf(stderr, "\033[31mUSFS Fatal Error: \033[0m");
        ret += vfprintf(stderr, fmt, ap);
        kill(0, SIGKILL);
        return ret;
    }

    assert(severity == Warn || severity == Log || severity == Debug);
    return vfprintf(usfs_log, fmt, ap);
}

int warn(const char *restrict fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = _log(error_severity == None ? None : Log, fmt, ap);
    va_end(ap);
    return ret;
}

int error(const char *restrict fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = _log(error_severity, fmt, ap);
    va_end(ap);
    return ret;
}
