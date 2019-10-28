all: main.c
	clang main.c -g -O0 -std=c11 -Wall -Wextra -Wpedantic