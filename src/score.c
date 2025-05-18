#include "score.h"
#include "shared.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static int list_hunt_ids(char ids[][128], int max) {
  DIR *d = opendir("hunt");
  if (!d) {
    perror("opendir hunts");
    return 0;
  }
  int n = 0;
  struct dirent *e;
  while ((e = readdir(d)) && n < max) {
    if (e->d_type == DT_DIR && strcmp(e->d_name, ".") &&
        strcmp(e->d_name, "..")) {
      strncpy(ids[n], e->d_name, 127);
      ids[n][127] = '\0';
      ++n;
    }
  }
  closedir(d);
  return n;
}

static void run_score_for(const char *hunt_id) {
  int pfd[2];
  if (pipe(pfd) == -1) {
    perror("pipe score");
    return;
  }
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork score");
    return;
  } else if (pid == 0) {
    close(pfd[PIPE_READ]);
    dup2(pfd[PIPE_WRITE], STDOUT_FILENO);
    dup2(pfd[PIPE_WRITE], STDERR_FILENO);
    close(pfd[PIPE_WRITE]);
    execl("./score_calc", "score_calc", hunt_id, NULL);
    perror("execl score_calc");
    _exit(1);
  }
  close(pfd[PIPE_WRITE]);
  printf("\n===== Scores for hunt \"%s\" =====\n", hunt_id);
  fflush(stdout);
  char buf[256];
  ssize_t n;
  while ((n = read(pfd[PIPE_READ], buf, sizeof(buf))) > 0)
    write(STDOUT_FILENO, buf, (size_t)n);
  close(pfd[PIPE_READ]);
  waitpid(pid, NULL, 0);
  puts("================================");
  fflush(stdout);
}

void calculate_scores(void) {
  char ids[128][128];
  int cnt = list_hunt_ids(ids, 128);
  if (cnt == 0) {
    puts("No hunts found.");
    return;
  }
  for (int i = 0; i < cnt; ++i)
    run_score_for(ids[i]);
}
