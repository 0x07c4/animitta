#define _GNU_SOURCE
#include "pty.h"

#include <fcntl.h>
#include <pty.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

int ct_pty_spawn_shell(CtPtySession *session) {
  int master = -1;
  pid_t pid = 0;
  int flags = 0;

  if (!session) {
    return -1;
  }

  pid = forkpty(&master, NULL, NULL, NULL);
  if (pid < 0) {
    return -1;
  }

  if (pid == 0) {
    char *argv[] = {"bash", "-i", NULL};
    execve("/bin/bash", argv, environ);
    _exit(127);
  }

  flags = fcntl(master, F_GETFL, 0);
  if (flags >= 0) {
    (void)fcntl(master, F_SETFL, flags | O_NONBLOCK);
  }

  session->master_fd = master;
  session->child_pid = pid;
  session->child_alive = 1;
  return 0;
}

int ct_pty_check_alive(CtPtySession *session) {
  int wstatus = 0;
  pid_t w = 0;

  if (!session || !session->child_alive) {
    return 0;
  }

  w = waitpid(session->child_pid, &wstatus, WNOHANG);
  if (w == session->child_pid) {
    session->child_alive = 0;
    return 0;
  }
  return 1;
}

ssize_t ct_pty_read(CtPtySession *session, void *buf, size_t n) {
  if (!session || !session->child_alive || session->master_fd < 0) {
    return -1;
  }
  return read(session->master_fd, buf, n);
}

ssize_t ct_pty_write(CtPtySession *session, const void *buf, size_t n) {
  if (!session || !session->child_alive || session->master_fd < 0) {
    return -1;
  }
  return write(session->master_fd, buf, n);
}

void ct_pty_close(CtPtySession *session) {
  int status = 0;

  if (!session) {
    return;
  }

  if (session->master_fd >= 0) {
    close(session->master_fd);
    session->master_fd = -1;
  }

  if (session->child_alive) {
    (void)waitpid(session->child_pid, &status, 0);
    session->child_alive = 0;
  }
}
