CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Werror -D_XOPEN_SOURCE -g
LDFLAGS=-lm

clean:
	rm -rf hsort quick
