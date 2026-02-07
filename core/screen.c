#include "screen.h"

#include <string.h>

void ct_screen_init(CtScreen *screen) {
  if (!screen) {
    return;
  }
  screen->len = 0;
  screen->ring[0] = '\0';
}

void ct_screen_clear(CtScreen *screen) {
  ct_screen_init(screen);
}

void ct_screen_backspace(CtScreen *screen) {
  if (!screen || screen->len == 0) {
    return;
  }
  if (screen->ring[screen->len - 1] == '\n') {
    return;
  }
  screen->len--;
  screen->ring[screen->len] = '\0';
}

void ct_screen_push(CtScreen *screen, char c) {
  if (!screen) {
    return;
  }
  if (screen->len + 1 >= CT_SCREEN_RING_SIZE) {
    memmove(screen->ring, screen->ring + 1, screen->len - 1);
    screen->len--;
  }
  screen->ring[screen->len++] = c;
  screen->ring[screen->len] = '\0';
}

const char *ct_screen_data(const CtScreen *screen) {
  if (!screen) {
    return "";
  }
  return screen->ring;
}

size_t ct_screen_len(const CtScreen *screen) {
  if (!screen) {
    return 0;
  }
  return screen->len;
}
