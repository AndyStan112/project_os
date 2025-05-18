CC      = gcc
CFLAGS  = -Wall -Wextra -Iinclude

TREASURE_HUNT_SRCS = src/treasure_hunt.c src/file_operations.c src/menu.c
TREASURE_HUB_SRCS  = src/treasure_hub.c src/shared.c src/score.c
MONITOR_SRCS       = src/monitor.c src/shared.c src/file_operations.c src/menu.c
SCORE_CALC_SRCS    = src/score_service.c

TREASURE_HUNT_TARGET = treasure_hunt
TREASURE_HUB_TARGET  = treasure_hub
MONITOR_TARGET       = monitor
SCORE_CALC_TARGET    = score_calc

all: $(TREASURE_HUNT_TARGET) $(TREASURE_HUB_TARGET) $(MONITOR_TARGET) $(SCORE_CALC_TARGET)

$(TREASURE_HUNT_TARGET): $(TREASURE_HUNT_SRCS)
	$(CC) $(CFLAGS) $^ -o $@

$(TREASURE_HUB_TARGET): $(TREASURE_HUB_SRCS)
	$(CC) $(CFLAGS) $^ -o $@

$(MONITOR_TARGET): $(MONITOR_SRCS)
	$(CC) $(CFLAGS) $^ -o $@

$(SCORE_CALC_TARGET): $(SCORE_CALC_SRCS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TREASURE_HUNT_TARGET) $(TREASURE_HUB_TARGET) $(MONITOR_TARGET) $(SCORE_CALC_TARGET)
