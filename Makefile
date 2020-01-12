all: main.c
	clang main.c -g -O3 -std=c11 -Wall -Wextra -Wpedantic
