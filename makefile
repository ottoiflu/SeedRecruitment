CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

.PHONY: level1
level1: level1.c
	@echo "Compiling level1..."
	$(CC) $(CFLAGS) level1.c -o ./level1
	@echo "level1 compiled successfully."
	./level1