#!/bin/bash

gcc -c doubll2d.c && ar rcs liblist.a doubll2d.o
gcc test.c -o staticlist -L ./ -llist
