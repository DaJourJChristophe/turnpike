#!/bin/bash

set -e

/usr/bin/gcc -c -Iinclude -fPIC -o src/queue.o src/queue.c

/usr/bin/gcc -shared -o libexec/libturnpike.so \
  src/queue.o


/usr/bin/gcc -c -Iinclude -o test/queue_test.o test/queue_test.c
/usr/bin/gcc -Llibexec -o bin/queue_test test/queue_test.o -lcmocka -lturnpike


/usr/bin/gcc -c -Iinclude -s -o examples/basic.o examples/basic.c
/usr/bin/gcc -Llibexec -o bin/basic examples/basic.o -lturnpike

rm -rf examples/*.o src/*.o test/*.o
