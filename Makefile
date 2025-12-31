CC := gcc
AR := ar
CFLAGS := -Wall -Wextra -Werror -I. -Iraylib/src

all: libraytext.a

raytext.o: raytext.c raytext.h
	$(CC) $(CFLAGS) -c -o $@ $<

libraytext.a: raytext.o
	$(AR) rcs $@ $^
