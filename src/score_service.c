#include "shared.h"
#include "treasure.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct score {
  char user[USERNAME_MAX];
  long sum;
};

static int find_user(struct score *a, int n, const char *u) {
  for (int i = 0; i < n; ++i)
    if (strncmp(a[i].user, u, USERNAME_MAX) == 0)
      return i;
  return -1;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s <hunt_id>\n", argv[0]);
    return 1;
  }

  char path[256];
  snprintf(path, sizeof(path), "%s/%s/%s", HUNT_DIR, argv[1], TREASURE_FILE);

  FILE *fp = fopen(path, "rb");
  if (!fp) {
    perror(path);
    return 1;
  }

  struct score scores[1024];
  memset(scores, 0, sizeof(scores));
  int n = 0;

  treasure_t t;
  while (fread(&t, sizeof(t), 1, fp) == 1) {
    int i = find_user(scores, n, t.username);
    if (i < 0) {
      i = n++;
      strncpy(scores[i].user, t.username, USERNAME_MAX - 1);
      scores[i].user[USERNAME_MAX - 1] = '\0';
      scores[i].sum = 0;
    }
    scores[i].sum += t.value;
  }
  fclose(fp);

  for (int i = 0; i < n; ++i)
    printf("%s,%ld\n", scores[i].user, scores[i].sum);

  return 0;
}
