#include "screen.h"

#include <string.h>

static CtScreenBuffer *ct_screen_active(CtScreen *screen) {
  if (!screen) {
    return NULL;
  }
  return screen->use_alt ? &screen->alt_buf : &screen->main_buf;
}

static const CtScreenBuffer *ct_screen_active_const(const CtScreen *screen) {
  if (!screen) {
    return NULL;
  }
  return screen->use_alt ? &screen->alt_buf : &screen->main_buf;
}

static void ct_buf_init(CtScreenBuffer *buf) {
  if (!buf) {
    return;
  }
  buf->len = 0;
  buf->cursor = 0;
  buf->line_start = 0;
  buf->ring[0] = '\0';
}

static void ct_buf_recompute_line_start(CtScreenBuffer *buf) {
  size_t i = 0;

  if (!buf) {
    return;
  }
  if (buf->cursor > buf->len) {
    buf->cursor = buf->len;
  }

  i = buf->cursor;
  while (i > 0) {
    if (buf->ring[i - 1] == '\n') {
      break;
    }
    i--;
  }
  buf->line_start = i;
}

static void ct_buf_discard_front(CtScreenBuffer *buf) {
  if (!buf || buf->len == 0) {
    return;
  }

  if (buf->len > 1) {
    memmove(buf->ring, buf->ring + 1, buf->len - 1);
  }
  buf->len--;
  buf->ring[buf->len] = '\0';

  if (buf->cursor > 0) {
    buf->cursor--;
  }
  if (buf->line_start > 0) {
    buf->line_start--;
  }
  ct_buf_recompute_line_start(buf);
}

static void ct_buf_ensure_room(CtScreenBuffer *buf, size_t needed) {
  while (buf && buf->len + needed >= CT_SCREEN_RING_SIZE) {
    ct_buf_discard_front(buf);
  }
}

static void ct_buf_insert_at(CtScreenBuffer *buf, size_t pos, char c) {
  if (!buf) {
    return;
  }
  if (pos > buf->len) {
    pos = buf->len;
  }
  ct_buf_ensure_room(buf, 1);

  if (pos < buf->len) {
    memmove(buf->ring + pos + 1, buf->ring + pos, buf->len - pos);
  }
  buf->ring[pos] = c;
  buf->len++;
  buf->ring[buf->len] = '\0';
}

static size_t ct_buf_line_end(const CtScreenBuffer *buf, size_t line_start) {
  size_t i = line_start;

  if (!buf) {
    return 0;
  }
  if (i > buf->len) {
    i = buf->len;
  }
  while (i < buf->len && buf->ring[i] != '\n') {
    i++;
  }
  return i;
}

static size_t ct_buf_prev_line_start(const CtScreenBuffer *buf, size_t line_start) {
  size_t i = line_start;

  if (!buf || i == 0) {
    return 0;
  }

  i--;
  if (i > 0 && buf->ring[i] == '\n') {
    i--;
  }
  while (i > 0 && buf->ring[i - 1] != '\n') {
    i--;
  }
  return i;
}

static size_t ct_buf_next_line_start(const CtScreenBuffer *buf, size_t line_start) {
  size_t end = 0;

  if (!buf) {
    return 0;
  }
  end = ct_buf_line_end(buf, line_start);
  if (end < buf->len && buf->ring[end] == '\n') {
    return end + 1;
  }
  return line_start;
}

static size_t ct_buf_cursor_col(const CtScreenBuffer *buf) {
  if (!buf || buf->cursor < buf->line_start) {
    return 0;
  }
  return buf->cursor - buf->line_start;
}

void ct_screen_init(CtScreen *screen) {
  if (!screen) {
    return;
  }
  ct_buf_init(&screen->main_buf);
  ct_buf_init(&screen->alt_buf);
  screen->use_alt = 0;
}

void ct_screen_clear(CtScreen *screen) {
  CtScreenBuffer *buf = ct_screen_active(screen);
  ct_buf_init(buf);
}

void ct_screen_erase_display(CtScreen *screen, int mode) {
  CtScreenBuffer *buf = ct_screen_active(screen);
  size_t erase_start = 0;
  size_t erase_end = 0;
  size_t erased = 0;

  if (!buf) {
    return;
  }

  if (mode == 2 || mode == 3) {
    ct_buf_init(buf);
    return;
  }

  if (mode == 1) {
    erase_start = 0;
    erase_end = buf->cursor;
  } else {
    erase_start = buf->cursor;
    erase_end = buf->len;
  }

  if (erase_end <= erase_start) {
    return;
  }

  erased = erase_end - erase_start;
  memmove(buf->ring + erase_start, buf->ring + erase_end, buf->len - erase_end + 1);
  buf->len -= erased;
  if (buf->cursor > erase_start) {
    buf->cursor = erase_start;
  }
  ct_buf_recompute_line_start(buf);
}

void ct_screen_erase_line(CtScreen *screen, int mode) {
  CtScreenBuffer *buf = ct_screen_active(screen);
  size_t line_end = 0;
  size_t erase_start = 0;
  size_t erase_end = 0;
  size_t erased = 0;

  if (!buf) {
    return;
  }
  line_end = ct_buf_line_end(buf, buf->line_start);

  if (mode == 1) {
    erase_start = buf->line_start;
    erase_end = buf->cursor;
  } else if (mode == 2) {
    erase_start = buf->line_start;
    erase_end = line_end;
  } else {
    erase_start = buf->cursor;
    erase_end = line_end;
  }

  if (erase_end <= erase_start) {
    return;
  }

  erased = erase_end - erase_start;
  memmove(buf->ring + erase_start, buf->ring + erase_end, buf->len - erase_end + 1);
  buf->len -= erased;

  if (buf->cursor > erase_end) {
    buf->cursor -= erased;
  } else if (buf->cursor > erase_start) {
    buf->cursor = erase_start;
  }
  ct_buf_recompute_line_start(buf);
}

void ct_screen_backspace(CtScreen *screen) {
  CtScreenBuffer *buf = ct_screen_active(screen);
  size_t del_pos = 0;

  if (!buf || buf->cursor == 0 || buf->cursor <= buf->line_start) {
    return;
  }
  del_pos = buf->cursor - 1;
  if (del_pos + 1 < buf->len) {
    memmove(buf->ring + del_pos, buf->ring + del_pos + 1, buf->len - (del_pos + 1));
  }
  buf->len--;
  buf->cursor--;
  buf->ring[buf->len] = '\0';
  ct_buf_recompute_line_start(buf);
}

void ct_screen_carriage_return(CtScreen *screen) {
  CtScreenBuffer *buf = ct_screen_active(screen);

  if (!buf) {
    return;
  }
  buf->cursor = buf->line_start;
}

void ct_screen_newline(CtScreen *screen) {
  CtScreenBuffer *buf = ct_screen_active(screen);
  size_t line_end = 0;

  if (!buf) {
    return;
  }

  // Treat LF as moving to next line boundary of the current row.
  // This avoids inserting '\n' at column 0 after CR, which caused rows to drift down.
  line_end = ct_buf_line_end(buf, buf->line_start);
  if (line_end < buf->len && buf->ring[line_end] == '\n') {
    buf->cursor = line_end + 1;
  } else {
    ct_buf_insert_at(buf, line_end, '\n');
    buf->cursor = line_end + 1;
  }
  buf->line_start = buf->cursor;
}

void ct_screen_write_char(CtScreen *screen, char c) {
  CtScreenBuffer *buf = ct_screen_active(screen);

  if (!buf) {
    return;
  }

  if (buf->cursor < buf->len) {
    if (buf->ring[buf->cursor] == '\n') {
      ct_buf_insert_at(buf, buf->cursor, c);
    } else {
      buf->ring[buf->cursor] = c;
    }
    buf->cursor++;
    ct_buf_recompute_line_start(buf);
    return;
  }

  ct_buf_ensure_room(buf, 1);
  buf->ring[buf->len++] = c;
  buf->ring[buf->len] = '\0';
  buf->cursor = buf->len;
  ct_buf_recompute_line_start(buf);
}

void ct_screen_cursor_set(CtScreen *screen, int row, int col) {
  CtScreenBuffer *buf = ct_screen_active(screen);
  int target_row = 1;
  int target_col = 1;
  size_t row_start = 0;
  int r = 1;
  size_t row_end = 0;

  if (!buf) {
    return;
  }

  if (row > 0) {
    target_row = row;
  }
  if (col > 0) {
    target_col = col;
  }

  while (r < target_row) {
    row_end = ct_buf_line_end(buf, row_start);
    if (row_end >= buf->len) {
      ct_buf_insert_at(buf, buf->len, '\n');
      row_start = buf->len;
    } else {
      row_start = row_end + 1;
    }
    r++;
  }

  row_end = ct_buf_line_end(buf, row_start);
  buf->line_start = row_start;
  {
    size_t col0 = (size_t)(target_col - 1);
    size_t line_len = row_end - row_start;
    if (col0 > line_len) {
      col0 = line_len;
    }
    buf->cursor = row_start + col0;
  }
}

void ct_screen_cursor_up(CtScreen *screen, int n) {
  CtScreenBuffer *buf = ct_screen_active(screen);
  int steps = 1;
  size_t col = 0;
  size_t row_start = 0;
  size_t prev_start = 0;
  size_t prev_end = 0;

  if (!buf) {
    return;
  }
  if (n > 0) {
    steps = n;
  }

  col = ct_buf_cursor_col(buf);
  row_start = buf->line_start;
  while (steps-- > 0 && row_start > 0) {
    row_start = ct_buf_prev_line_start(buf, row_start);
  }
  prev_start = row_start;
  prev_end = ct_buf_line_end(buf, prev_start);
  if (col > prev_end - prev_start) {
    col = prev_end - prev_start;
  }
  buf->line_start = prev_start;
  buf->cursor = prev_start + col;
}

void ct_screen_cursor_down(CtScreen *screen, int n) {
  CtScreenBuffer *buf = ct_screen_active(screen);
  int steps = 1;
  size_t col = 0;
  size_t row_start = 0;
  size_t next_start = 0;
  size_t next_end = 0;

  if (!buf) {
    return;
  }
  if (n > 0) {
    steps = n;
  }

  col = ct_buf_cursor_col(buf);
  row_start = buf->line_start;
  while (steps-- > 0) {
    next_start = ct_buf_next_line_start(buf, row_start);
    if (next_start == row_start) {
      break;
    }
    row_start = next_start;
  }
  next_end = ct_buf_line_end(buf, row_start);
  if (col > next_end - row_start) {
    col = next_end - row_start;
  }
  buf->line_start = row_start;
  buf->cursor = row_start + col;
}

void ct_screen_cursor_forward(CtScreen *screen, int n) {
  CtScreenBuffer *buf = ct_screen_active(screen);
  int steps = 1;
  size_t line_end = 0;
  size_t target = 0;

  if (!buf) {
    return;
  }
  if (n > 0) {
    steps = n;
  }

  line_end = ct_buf_line_end(buf, buf->line_start);
  target = buf->cursor + (size_t)steps;
  if (target > line_end) {
    target = line_end;
  }
  buf->cursor = target;
}

void ct_screen_cursor_back(CtScreen *screen, int n) {
  CtScreenBuffer *buf = ct_screen_active(screen);
  int steps = 1;
  size_t start = 0;
  size_t target = 0;

  if (!buf) {
    return;
  }
  if (n > 0) {
    steps = n;
  }

  start = buf->line_start;
  if (buf->cursor <= start) {
    return;
  }
  if ((size_t)steps > buf->cursor - start) {
    target = start;
  } else {
    target = buf->cursor - (size_t)steps;
  }
  buf->cursor = target;
}

void ct_screen_set_alternate(CtScreen *screen, int enabled, int clear_on_enter) {
  if (!screen) {
    return;
  }
  if (enabled) {
    if (clear_on_enter) {
      ct_buf_init(&screen->alt_buf);
    }
    screen->use_alt = 1;
    return;
  }
  screen->use_alt = 0;
}

const char *ct_screen_data(const CtScreen *screen) {
  const CtScreenBuffer *buf = ct_screen_active_const(screen);
  if (!buf) {
    return "";
  }
  return buf->ring;
}

size_t ct_screen_len(const CtScreen *screen) {
  const CtScreenBuffer *buf = ct_screen_active_const(screen);
  if (!buf) {
    return 0;
  }
  return buf->len;
}
