#include "file_operations.h"
#include "shared.h"
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

volatile sig_atomic_t got_command = 0;
volatile sig_atomic_t should_stop = 0;

void handle_command_signal(int sig) {
  (void)sig;
  got_command = 1;
}

void handle_stop_signal(int sig) {
  (void)sig;
  should_stop = 1;
}

int main() {
  setvbuf(stdout, NULL, _IOLBF, 0);
  setvbuf(stderr, NULL, _IOLBF, 0);

  struct sigaction sa_cmd, sa_stop;
  sa_cmd.sa_handler = handle_command_signal;
  sa_stop.sa_handler = handle_stop_signal;
  sigemptyset(&sa_cmd.sa_mask);
  sigemptyset(&sa_stop.sa_mask);
  sa_cmd.sa_flags = 0;
  sa_stop.sa_flags = 0;

  sigaction(SIG_CMD, &sa_cmd, NULL);
  sigaction(SIG_STOP, &sa_stop, NULL);

  printf("Monitor started. PID: %d\n", getpid());

  while (!should_stop) {
    pause();

    if (got_command) {
      got_command = 0;

      char command[MAX_CMD_LEN];
      read_command_from_file(command, sizeof(command));

      if (strncmp(command, "list_hunts", 10) == 0) {
        list_hunts();

      } else if (strncmp(command, "list_treasures", 14) == 0) {
        char hunt_id[128];
        if (sscanf(command, "list_treasures %s", hunt_id) == 1) {
          list_treasures(hunt_id);
        } else {
          fprintf(stderr, "Invalid command format.\n");
        }

      } else if (strncmp(command, "view_treasure", 13) == 0) {
        char hunt_id[128];
        int treasure_id;
        if (sscanf(command, "view_treasure %s %d", hunt_id, &treasure_id) ==
            2) {
          if (view_treasure(hunt_id, treasure_id) != 0) {
            fprintf(stderr, "Treasure %d not found.\n", treasure_id);
          }
        } else {
          fprintf(stderr, "Invalid command format.\n");
        }

      } else {
        fprintf(stderr, "Unknown command: %s\n", command);
      }
    }
  }

  usleep(2000000);
  printf("Monitor exiting.\n");
  return 0;
}
