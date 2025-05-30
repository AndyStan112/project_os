#include "file_operations.h"
#include "shared.h"
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

int log_operation(const char *hunt_id, int treasure_id, const char *operation) {
  char filepath[256];
  snprintf(filepath, sizeof(filepath), "%s/%s/%s", HUNT_DIR, hunt_id, LOG_FILE);

  int fd = open(filepath, O_WRONLY | O_APPEND | O_CREAT, 0644);
  if (fd < 0) {
    perror("open log file");
    return -1;
  }

  time_t now = time(NULL);
  char timebuf[64];
  strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", localtime(&now));

  char log_entry[512];
  if (treasure_id == -1) {
    snprintf(log_entry, sizeof(log_entry), "[%s] Performed action %s\n",
             timebuf, operation);
  } else {
    snprintf(log_entry, sizeof(log_entry),
             "[%s] Performed action %s on treasure with id=%d\n", timebuf,
             operation, treasure_id);
  }

  if (write(fd, log_entry, strlen(log_entry)) != (ssize_t)strlen(log_entry)) {
    perror("write log");
    close(fd);
    return -1;
  }
  close(fd);
  return 0;
}

int create_hunt_directory(const char *hunt_id) {
  struct stat st;

  if (stat(HUNT_DIR, &st) == -1) {
    if (mkdir(HUNT_DIR, 0755) == -1) {
      perror("mkdir hunt");
      return -1;
    }
  }

  char hunt_path[256];
  snprintf(hunt_path, sizeof(hunt_path), "%s/%s", HUNT_DIR, hunt_id);
  if (stat(hunt_path, &st) == -1) {
    if (mkdir(hunt_path, 0755) == -1) {
      perror("mkdir hunt_id");
      return -1;
    }
  }

  return 0;
}

int create_symlink_for_log(const char *hunt_id) {
  char target[256];
  char linkname[256];

  snprintf(target, sizeof(target), "%s/%s/%s", HUNT_DIR, hunt_id, LOG_FILE);
  snprintf(linkname, sizeof(linkname), "logged_hunt-%s", hunt_id);

  unlink(linkname);

  if (symlink(target, linkname) != 0) {
    perror("symlink");
    return -1;
  }

  return 0;
}

int add_treasure_to_file(const char *hunt_id, const treasure_t *treasure) {
  char filepath[256];
  snprintf(filepath, sizeof(filepath), "%s/%s/%s", HUNT_DIR, hunt_id,
           TREASURE_FILE);

  int fd = open(filepath, O_WRONLY | O_APPEND | O_CREAT, 0644);
  if (fd < 0) {
    perror("open treasure file");
    return -1;
  }

  if (write(fd, treasure, sizeof(treasure_t)) != sizeof(treasure_t)) {
    perror("write treasure");
    close(fd);
    return -1;
  }
  close(fd);

  if (log_operation(hunt_id, treasure->treasure_id, "add treasure to file") !=
      0)
    fprintf(stderr, "Logging failed in add_treasure_to_file\n");

  return 0;
}

int list_treasures(const char *hunt_id) {
  char filepath[256];
  snprintf(filepath, sizeof(filepath), "%s/%s/%s", HUNT_DIR, hunt_id,
           TREASURE_FILE);

  struct stat st;
  if (stat(filepath, &st) == -1) {
    perror("stat treasure file");
    return -1;
  }

  printf("Hunt: %s\n", hunt_id);
  printf("Treasure file: %s\n", filepath);
  printf("Size: %ld bytes\n", st.st_size);
  printf("Last modified: %s", ctime(&st.st_mtime));

  int fd = open(filepath, O_RDONLY);
  if (fd < 0) {
    perror("open treasure file");
    return -1;
  }

  treasure_t t;
  printf("\nList of treasures:\n");
  while (read(fd, &t, sizeof(treasure_t)) == sizeof(treasure_t)) {
    printf("  ID: %d\n", t.treasure_id);
    printf("  User: %s\n", t.username);
    printf("  Latitude: %.6f\n", t.latitude);
    printf("  Longitude: %.6f\n", t.longitude);
    printf("  Value: %d\n", t.value);
    printf("  Clue: %s\n", t.clue);
    printf("-------------------------\n");
  }
  close(fd);

  if (log_operation(hunt_id, -1, "list treasures") != 0)
    fprintf(stderr, "Logging failed in list_treasures\n");

  return 0;
}
int view_treasure(const char *hunt_id, int treasure_id) {
  char filepath[256];
  snprintf(filepath, sizeof(filepath), "%s/%s/%s", HUNT_DIR, hunt_id,
           TREASURE_FILE);

  int fd = open(filepath, O_RDONLY);
  if (fd < 0) {
    perror("open treasure file");
    return -1;
  }

  treasure_t t;
  int found = 0;
  while (read(fd, &t, sizeof(treasure_t)) == sizeof(treasure_t)) {
    if (t.treasure_id == treasure_id) {
      if (!found) {
        printf("Treasure Details:\n");
        found = 1;
      }
      printf("  ID: %d\n", t.treasure_id);
      printf("  User: %s\n", t.username);
      printf("  Latitude: %.6f\n", t.latitude);
      printf("  Longitude: %.6f\n", t.longitude);
      printf("  Value: %d\n", t.value);
      printf("  Clue: %s\n", t.clue);
      printf("-------------------------\n");
    }
  }
  close(fd);

  if (!found)
    return -1;

  if (log_operation(hunt_id, treasure_id, "view treasure") != 0)
    fprintf(stderr, "Logging failed in view_treasure\n");

  return 0;
}

int remove_treasure(const char *hunt_id, int treasure_id) {
  char filepath[256];
  snprintf(filepath, sizeof(filepath), "%s/%s/%s", HUNT_DIR, hunt_id,
           TREASURE_FILE);

  int fd = open(filepath, O_RDONLY);
  if (fd < 0) {
    perror("open treasure file");
    return -1;
  }

  treasure_t t;
  char *buffer = NULL;
  int found = 0;
  ssize_t write_offset = 0;
  while (read(fd, &t, sizeof(treasure_t)) == sizeof(treasure_t)) {
    if (t.treasure_id == treasure_id) {
      found = 1;
      continue;
    }
    char *tmp = realloc(buffer, write_offset + sizeof(treasure_t));
    if (!tmp) {
      free(buffer);
      close(fd);
      perror("realloc");
      return -1;
    }
    buffer = tmp;
    memcpy(buffer + write_offset, &t, sizeof(treasure_t));
    write_offset += sizeof(treasure_t);
  }
  close(fd);

  if (!found) {
    free(buffer);
    return -1;
  }

  int out = open(filepath, O_WRONLY | O_TRUNC);
  if (out < 0) {
    perror("open treasure file");
    free(buffer);
    return -1;
  }

  if (write(out, buffer, write_offset) != write_offset) {
    perror("write treasure");
    close(out);
    free(buffer);
    return -1;
  }
  close(out);
  free(buffer);

  if (log_operation(hunt_id, treasure_id, "remove") != 0)
    fprintf(stderr, "Logging failed in remove_treasure\n");

  return 0;
}

int remove_hunt(const char *hunt_id) {
  char path[256];

  snprintf(path, sizeof(path), "%s/%s/%s", HUNT_DIR, hunt_id, TREASURE_FILE);
  if (unlink(path) != 0)
    perror("unlink treasures.dat");

  snprintf(path, sizeof(path), "%s/%s/%s", HUNT_DIR, hunt_id, LOG_FILE);
  if (unlink(path) != 0)
    perror("unlink logged_hunt");

  snprintf(path, sizeof(path), "%s/%s", HUNT_DIR, hunt_id);
  if (rmdir(path) != 0)
    perror("rmdir hunt dir");

  snprintf(path, sizeof(path), "logged_hunt-%s", hunt_id);
  unlink(path);

  return 0;
}

int list_hunts() {
  DIR *dir = opendir(HUNT_DIR);
  if (!dir) {
    perror("opendir hunt");
    return -1;
  }

  struct dirent *entry;
  while ((entry = readdir(dir))) {
    if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 &&
        strcmp(entry->d_name, "..") != 0) {

      char filepath[512];
      snprintf(filepath, sizeof(filepath), "%s/%s/%s", HUNT_DIR, entry->d_name,
               TREASURE_FILE);
      int fd = open(filepath, O_RDONLY);
      if (fd < 0)
        continue;

      int count = 0;
      treasure_t t;
      while (read(fd, &t, sizeof(treasure_t)) == sizeof(treasure_t))
        count++;
      close(fd);

      printf("Hunt ID: %s\n", entry->d_name);
      printf("  Total treasures: %d\n", count);
    }
  }

  closedir(dir);
  return 0;
}
