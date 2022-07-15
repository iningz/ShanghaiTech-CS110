#!/bin/bash

gcc -c -fPIC doubll2d.c && gcc -shared -o liblist.so doubll2d.o
gcc test.c -o dynamiclist -L ./ -llist -Wl,-rpath,.
