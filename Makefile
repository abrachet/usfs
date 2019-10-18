# Part of USFS
# Alex Brachet 
SRCS = src/syscall.c $(wildcard src/*.c) src/syscall.c 

HEADERS = $(wildcard include/usfs/*.h)

DEFINES =
CFLAGS = -fPIC -Wall -Wvla -Wno-unused-function -Wextra -Wno-sign-compare



include config.mak

OBJS = $(SRCS:%.c=%.o)
export OBJS

all: usfs.so

src/syscall.c include/usfs/syscall.h: utils/syscalls.py
	./utils/make_syscall_files include/usfs/syscall.h src/syscall.c

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@ -Iinclude

test: $(OBJS)
	$(MAKE) -C test/ test

debug: $(OBJS)
	$(MAKE) -C test/ debug

usfs.so: $(OBJS)
	$(CC) -shared $(OBJS) -o usfs.so

FMT := $(SRCS:%.c=%.clang_format)
%.clang_format: %.c 
	clang-format -i $<
format: $(FMT)

install: usfs.so
	cp usfs.so $(LIBDIR)/usfs.so

clean:
	rm -rf $(OBJS)
	rm -rf *.dSYM
	rm -rf utils/*.pyc
	rm -rf include/usfs/syscall.h src/syscall.c
	$
	$(MAKE) -C test/ clean
