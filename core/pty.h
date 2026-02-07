#ifndef CUTERM_CORE_PTY_H
#define CUTERM_CORE_PTY_H

#include <stddef.h>
#include <sys/types.h>

typedef struct {
  int master_fd;
  pid_t child_pid;
  int child_alive;
} CtPtySession;

int ct_pty_spawn_shell(CtPtySession *session);
int ct_pty_check_alive(CtPtySession *session);
ssize_t ct_pty_read(CtPtySession *session, void *buf, size_t n);
ssize_t ct_pty_write(CtPtySession *session, const void *buf, size_t n);
void ct_pty_close(CtPtySession *session);

#endif
