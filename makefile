CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

.PHONY: level1
level1: level1.c
	@echo "Compiling level1..."
	$(CC) $(CFLAGS) level1.c -o ./level1
	@echo "level1 compiled successfully."
	./level1

.PHONY: level2
level2: level2.c
	@echo "Compiling level2..."
	$(CC) $(CFLAGS) level2.c -o ./level2
	@echo "level2 compiled successfully."
	./level2

.PHONY: level3
level3: level3.c
	@echo "Compiling level3..."
	$(CC) $(CFLAGS) level3.c -o ./level3
	@echo "level3 compiled successfully."
	./level3