#include <stddef.h>
#ifndef SHARED_H
#define SHARED_H

#define CMD_FILE "command.txt"
#define MAX_CMD_LEN 256

#define SIG_CMD SIGUSR1
#define SIG_STOP SIGUSR2
#define PIPE_READ 0
#define PIPE_WRITE 1

void write_command_to_file(const char *cmd);
void read_command_from_file(char *buffer, size_t size);

#endif
