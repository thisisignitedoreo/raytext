CC := gcc
AR := ar
CFLAGS := -Wall -Wextra -Werror -I. -Iraylib/src

all: libraytext.a example

raytext.o: raytext.c raytext.h
	$(CC) $(CFLAGS) -c -o $@ $<

libraytext.a: raytext.o
	$(AR) rcs $@ $^

example: example.c libraytext.a
	$(CC) $(CFLAGS) -o $@ example.c -L. -Lraylib/src -lraytext -lraylib -lm
