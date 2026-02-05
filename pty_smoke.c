// pty_smoke.c
#define _GNU_SOURCE
#include <poll.h>
#include <pty.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

int main() {
  int master = -1;
  pid_t pid = forkpty(&master, NULL, NULL, NULL);
  if (pid < 0) {
    perror("forkpty");
    return 1;
  }

  if (pid == 0) {
    char *argv[] = {"bash", "-i", NULL};
    execve("/bin/bash", argv, environ);
    perror("execve");
    _exit(127);
  }

  struct pollfd pfd = {.fd = master, .events = POLLIN};
  char buf[4096];

  while (1) {
    int r = poll(&pfd, 1, 1000);
    if (r < 0) {
      perror("poll");
      break;
    }
    if (r == 0)
      continue;

    if (pfd.revents & POLLIN) {
      ssize_t n = read(master, buf, sizeof(buf));
      if (n <= 0)
        break;
      fwrite(buf, 1, (size_t)n, stdout);
      fflush(stdout);
    }
  }

  int status = 0;
  waitpid(pid, &status, 0);
  return 0;
}
