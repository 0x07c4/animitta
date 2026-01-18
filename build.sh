#!/bin/sh

set -xe

clang -std=c11 -Wall -Wextra -O0 -g sdl_demo.c $(
  pkg-config --cflags --libs sdl2
) -o build/sdl_demo

clang -std=c11 -Wall -Wextra -O0 -g ppm_demo.c -o build/ppm_demo
