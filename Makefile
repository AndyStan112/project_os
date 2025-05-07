CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

TREASURE_HUNT_SRCS = src/treasure_hunt.c src/file_operations.c src/menu.c
TREASURE_HUB_SRCS = src/treasure_hub.c src/shared.c
MONITOR_SRCS = src/monitor.c src/shared.c src/file_operations.c src/menu.c

TREASURE_HUNT_TARGET = treasure_hunt
TREASURE_HUB_TARGET = treasure_hub
MONITOR_TARGET = monitor

all: $(TREASURE_HUNT_TARGET) $(TREASURE_HUB_TARGET) $(MONITOR_TARGET)

$(TREASURE_HUNT_TARGET): $(TREASURE_HUNT_SRCS)
	$(CC) $(CFLAGS) $^ -o $@

$(TREASURE_HUB_TARGET): $(TREASURE_HUB_SRCS)
	$(CC) $(CFLAGS) $^ -o $@

$(MONITOR_TARGET): $(MONITOR_SRCS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TREASURE_HUNT_TARGET) $(TREASURE_HUB_TARGET) $(MONITOR_TARGET)
