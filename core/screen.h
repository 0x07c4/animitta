#ifndef CUTERM_CORE_SCREEN_H
#define CUTERM_CORE_SCREEN_H

#include <stddef.h>

#define CT_SCREEN_RING_SIZE (1u << 20)

typedef struct {
  char ring[CT_SCREEN_RING_SIZE];
  size_t len;
} CtScreen;

void ct_screen_init(CtScreen *screen);
void ct_screen_clear(CtScreen *screen);
void ct_screen_backspace(CtScreen *screen);
void ct_screen_push(CtScreen *screen, char c);

const char *ct_screen_data(const CtScreen *screen);
size_t ct_screen_len(const CtScreen *screen);

#endif
