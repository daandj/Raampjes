CC = i686-raampjes-gcc
AS = nasm
AR = i686-raampjes-ar

FILES =	string/memcpy \
				string/memset \
				string/strlen \
				string/strncmp \
				stdlib/exit \
				unistd/execve \
				unistd/fork \
				unistd/read \
				unistd/write \

LIBC_OBJS = $(addsuffix .libc.o,$(addprefix build/,$(FILES)))
LIBK_OBJS = $(addsuffix .libk.o,$(addprefix build/,$(FILES)))

CFLAGS = -Wall -Wextra
LIBK_FLAGS = $(CFLAGS) -ffreestanding -D _IS_LIBK

.PHONY: all clean

all: build/libc.a build/libk.a build/i386/crt0.o

build/libc.a: $(LIBC_OBJS)
	$(AR) rcs $@ $^

build/libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $^

build/%.libc.o: lib/%.c
	-mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $^ -o $@

build/%.libk.o: lib/%.c
	-mkdir -p $(dir $@)
	$(CC) $(LIBK_FLAGS) -c $< -o $@

build/i386/crt0.o: i386/crt0.asm
	-mkdir -p $(dir $@)
	$(AS) -f elf -o $@ $<

clean:
	-rm -rf build/*
