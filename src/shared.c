#include "shared.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

void write_command_to_file(const char *cmd) {
    int fd = open(CMD_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open write");
        return;
    }

    size_t len = strlen(cmd);
    if (write(fd, cmd, len) != (ssize_t)len) {
        perror("write command");
        close(fd);
        return;
    }

    if (write(fd, "\n", 1) != 1) {
        perror("write newline");
    }

    close(fd);
}

void read_command_from_file(char *buffer, size_t size) {
    int fd = open(CMD_FILE, O_RDONLY);
    if (fd < 0) {
        perror("open read");
        if (size > 0) buffer[0] = '\0';
        return;
    }

    ssize_t bytes_read = read(fd, buffer, size - 1);
    if (bytes_read < 0) {
        perror("read command");
        buffer[0] = '\0';
        close(fd);
        return;
    }

    buffer[bytes_read] = '\0';

    char *newline = strchr(buffer, '\n');
    if (newline) *newline = '\0';

    close(fd);
}
