#ifndef CUTERM_CORE_SCREEN_H
#define CUTERM_CORE_SCREEN_H

#include <stddef.h>

#define CT_SCREEN_RING_SIZE (1u << 20)

typedef struct {
  char ring[CT_SCREEN_RING_SIZE];
  size_t len;
  size_t cursor;
  size_t line_start;
} CtScreenBuffer;

typedef struct {
  CtScreenBuffer main_buf;
  CtScreenBuffer alt_buf;
  int use_alt;
} CtScreen;

void ct_screen_init(CtScreen *screen);
void ct_screen_clear(CtScreen *screen);
void ct_screen_erase_display(CtScreen *screen, int mode);
void ct_screen_erase_line(CtScreen *screen, int mode);
void ct_screen_backspace(CtScreen *screen);
void ct_screen_carriage_return(CtScreen *screen);
void ct_screen_newline(CtScreen *screen);
void ct_screen_write_char(CtScreen *screen, char c);
void ct_screen_cursor_set(CtScreen *screen, int row, int col);
void ct_screen_cursor_up(CtScreen *screen, int n);
void ct_screen_cursor_down(CtScreen *screen, int n);
void ct_screen_cursor_forward(CtScreen *screen, int n);
void ct_screen_cursor_back(CtScreen *screen, int n);
void ct_screen_set_alternate(CtScreen *screen, int enabled, int clear_on_enter);

const char *ct_screen_data(const CtScreen *screen);
size_t ct_screen_len(const CtScreen *screen);

#endif
