#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include "shared.h"

pid_t monitor_pid = -1;
int monitor_terminating = 0;

void sigchld_handler(int sig) {
    (void)sig;
    int saved_errno = errno;
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        if (pid == monitor_pid) {
            printf("Monitor has terminated.\n");
            monitor_pid = -1;
            monitor_terminating = 0;
        }
    }
    errno = saved_errno;
}

void start_monitor() {
    if (monitor_pid > 0) {
        printf("Monitor already running.\n");
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
    } else if (pid == 0) {
        execl("./monitor", "monitor", NULL);
        perror("execl");
        exit(1);
    } else {
        monitor_pid = pid;
        printf("Monitor started with PID %d.\n", pid);
    }
}

void stop_monitor() {
    if (monitor_pid <= 0) {
        printf("Monitor not running.\n");
        return;
    }
    kill(monitor_pid, SIG_STOP);
    monitor_terminating = 1;
    printf("Sent stop signal to monitor. Waiting for it to terminate...\n");
}

void send_command(const char *command) {
    if (monitor_terminating) {
        printf("Monitor is shutting down. Please wait.\n");
        return;
    }
    if (monitor_pid <= 0) {
        printf("Monitor not running.\n");
        return;
    }

    write_command_to_file(command);
    kill(monitor_pid, SIG_CMD);
}

int main() {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    char input[128];
    while (1) {
        printf("hub> ");
        if (!fgets(input, sizeof(input), stdin)) break;

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "start_monitor") == 0) {
            start_monitor();
        } else if (strcmp(input, "stop_monitor") == 0) {
            stop_monitor();
        } else if (strcmp(input, "exit") == 0) {
            if (monitor_pid > 0) {
                printf("Error: Monitor still running.\n");
                continue;
            }
            break;
        } else if (strncmp(input, "list_hunts", 10) == 0 ||
                   strncmp(input, "list_treasures", 14) == 0 ||
                   strncmp(input, "view_treasure", 13) == 0) {
            send_command(input);
        } else {
            printf("Unknown command.\n");
        }
    }

    return 0;
}
