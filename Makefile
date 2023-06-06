SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c, build/%.o, $(SRCS))
CC := clang
CFLAGS := -Wall -Wextra -Werror -Wpedantic -g --std=c2x

TARGET := bin/main

all: $(TARGET)

bin/main: $(OBJS)
	$(CC) $^ -o $@
	
build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean