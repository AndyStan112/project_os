#include "score.h"
#include "shared.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

pid_t monitor_pid = -1;
int monitor_pipe_fd = -1;
int monitor_terminating = 0;

static void drain_monitor_output(void) {
  if (monitor_pipe_fd < 0)
    return;
  char buf[256];
  ssize_t n;
  while ((n = read(monitor_pipe_fd, buf, sizeof(buf))) > 0)
    write(STDOUT_FILENO, buf, (size_t)n);
}

void sigchld_handler(int sig) {
  (void)sig;
  int saved_errno = errno;
  pid_t pid;
  while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
    if (pid == monitor_pid) {
      printf("Monitor has terminated.\n");
      monitor_pid = -1;
      monitor_terminating = 0;
      if (monitor_pipe_fd >= 0) {
        close(monitor_pipe_fd);
        monitor_pipe_fd = -1;
      }
    }
  }
  errno = saved_errno;
}

void setup_sigchld_handler() {
  struct sigaction sa;
  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  sigaction(SIGCHLD, &sa, NULL);
}

void launch_monitor_process(int *pfd) {
  setsid();
  close(pfd[PIPE_READ]);
  dup2(pfd[PIPE_WRITE], STDOUT_FILENO);
  dup2(pfd[PIPE_WRITE], STDERR_FILENO);
  close(pfd[PIPE_WRITE]);
  close(STDIN_FILENO);
  execl("./monitor", "monitor", NULL);
  perror("execl");
  exit(1);
}

void start_monitor() {
  if (monitor_pid > 0) {
    printf("Monitor already running.\n");
    return;
  }
  int pfd[2];
  if (pipe(pfd) == -1) {
    perror("pipe");
    return;
  }
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    return;
  }
  if (pid == 0) {
    launch_monitor_process(pfd);
  } else {
    close(pfd[PIPE_WRITE]);
    monitor_pipe_fd = pfd[PIPE_READ];
    int flags = fcntl(monitor_pipe_fd, F_GETFL, 0);
    fcntl(monitor_pipe_fd, F_SETFL, flags | O_NONBLOCK);
    monitor_pid = pid;
    printf("Monitor starting with PID %d...\n", pid);
    usleep(50000);
    drain_monitor_output();
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
  usleep(20000);
  drain_monitor_output();
}

void handle_command(const char *input) {
  if (strcmp(input, "start_monitor") == 0) {
    start_monitor();
  } else if (strcmp(input, "stop_monitor") == 0) {
    stop_monitor();
  } else if (strcmp(input, "exit") == 0) {
    if (monitor_pid > 0) {
      printf("Error: Monitor still running.\n");
      return;
    }
    exit(0);
  } else if (strncmp(input, "list_hunts", 10) == 0 ||
             strncmp(input, "list_treasures", 14) == 0 ||
             strncmp(input, "view_treasure", 13) == 0) {
    send_command(input);
  } else if (strcmp(input, "calculate_score") == 0) {
    calculate_scores();
  } else {
    printf("Unknown command.\n");
  }
}

int main() {
  setup_sigchld_handler();
  char input[128];
  while (1) {
    drain_monitor_output();
    printf("hub> ");
    if (!fgets(input, sizeof(input), stdin))
      break;
    input[strcspn(input, "\n")] = '\0';
    handle_command(input);
  }
  return 0;
}
