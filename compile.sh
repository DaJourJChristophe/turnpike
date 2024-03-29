#!/bin/bash

set -e

/usr/bin/gcc -c -Iinclude -fPIC -o src/bipartite.o src/bipartite.c
/usr/bin/gcc -c -Iinclude -fPIC -o src/bipbuf.o src/bipbuf.c
/usr/bin/gcc -c -Iinclude -fPIC -o src/queue.o src/queue.c

/usr/bin/gcc -shared -o libexec/libturnpike.so \
  src/bipartite.o \
  src/bipbuf.o \
  src/queue.o

/usr/bin/gcc -c -Iinclude -o test/bipartite_test.o test/bipartite_test.c
/usr/bin/gcc -Llibexec -o bin/bipartite_test test/bipartite_test.o -lpthread -lcmocka -lturnpike -ljemalloc

/usr/bin/gcc -c -Iinclude -o test/queue_test.o test/queue_test.c
/usr/bin/gcc -Llibexec -o bin/queue_test test/queue_test.o -lpthread -lcmocka -lturnpike -ljemalloc

# /usr/bin/gcc -c -Iinclude -o test/tsqueue_test.o test/tsqueue_test.c
# /usr/bin/gcc -Llibexec -o bin/tsqueue_test test/tsqueue_test.o -lpthread -lcmocka -lturnpike -ljemalloc


/usr/bin/gcc -c -Iinclude -s -o examples/basic.o examples/basic.c
/usr/bin/gcc -Llibexec -o bin/basic examples/basic.o -lturnpike -ljemalloc

/usr/bin/gcc -c -Iinclude -s -o examples/bipbuf.o examples/bipbuf.c
/usr/bin/gcc -Llibexec -o bin/bipbuf examples/bipbuf.o -lturnpike -ljemalloc

/usr/bin/gcc -c -Iinclude -s -o examples/thread_safety.o examples/thread_safety.c
/usr/bin/gcc -Llibexec -o bin/thread_safety examples/thread_safety.o -lturnpike -ljemalloc

rm -rf examples/*.o src/*.o test/*.o
