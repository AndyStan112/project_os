CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

SRCS = src/main.c src/file_operations.c src/menu.c
TARGET = treasure_hunt

all:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)
