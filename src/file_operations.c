#include "file_operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

#define TREASURE_FILE "treasures.dat"
#define LOG_FILE      "logged_hunt"

int create_hunt_directory(const char *hunt_id) {
    struct stat st;

    if (stat("hunt", &st) == -1) {
        if (mkdir("hunt", 0755) == -1) {
            perror("mkdir hunt");
            return -1;
        }
    }

    char hunt_path[256];
    snprintf(hunt_path, sizeof(hunt_path), "hunt/%s", hunt_id);
    if (stat(hunt_path, &st) == -1) {
        if (mkdir(hunt_path, 0755) == -1) {
            perror("mkdir");
            return -1;
        }
    }
    return 0;
}

int create_symlink_for_log(const char *hunt_id) {
    char target[256];
    char linkname[256];

    snprintf(target, sizeof(target), "hunt/%s/%s", hunt_id, LOG_FILE);
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
    snprintf(filepath, sizeof(filepath), "hunt/%s/%s", hunt_id, TREASURE_FILE);

    int fd = open(filepath, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) {
        perror("open treasure file");
        return -1;
    }

    ssize_t bytes_written = write(fd, treasure, sizeof(treasure_t));
    if (bytes_written != sizeof(treasure_t)) {
        perror("write treasure");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int list_treasures(const char *hunt_id) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "hunt/%s/%s", hunt_id, TREASURE_FILE);

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
    return 0;
}

int view_treasure(const char *hunt_id, int treasure_id) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "hunt/%s/%s", hunt_id, TREASURE_FILE);

    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        perror("open treasure file");
        return -1;
    }

    treasure_t t;
    int found = 0;
    while (read(fd, &t, sizeof(treasure_t)) == sizeof(treasure_t)) {
        if (t.treasure_id == treasure_id) {
            printf("Treasure Details:\n");
            printf("  ID: %d\n", t.treasure_id);
            printf("  User: %s\n", t.username);
            printf("  Latitude: %.6f\n", t.latitude);
            printf("  Longitude: %.6f\n", t.longitude);
            printf("  Value: %d\n", t.value);
            printf("  Clue: %s\n", t.clue);
            found = 1;
            break;
        }
    }

    close(fd);

    if (!found) {
        return -1;
    }
    return 0;
}

int log_operation(const char *hunt_id, const char *operation) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "hunt/%s/%s", hunt_id, LOG_FILE);

    int fd = open(filepath, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) {
        perror("open log file");
        return -1;
    }

    time_t now = time(NULL);
    char timebuf[64];
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", localtime(&now));

    char log_entry[512];
    snprintf(log_entry, sizeof(log_entry), "[%s] %s\n", timebuf, operation);

    if (write(fd, log_entry, strlen(log_entry)) != (ssize_t)strlen(log_entry)) {
        perror("write log");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}
