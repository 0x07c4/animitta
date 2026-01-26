#ifndef PENCIL_C_
#define PENCIL_C_

#include "base/log.h"
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PENCILDEF static inline

#define PENCIL_GRAY 0xFF202020
#define PENCIL_RED 0xFF0000FF
#define PENCIL_GREEN 0xFF00FF00
#define PENCIL_BLUE 0XFFFF0000

#define return_defer(value)                                                    \
  do {                                                                         \
    result = value;                                                            \
    goto defer;                                                                \
  } while (0)

typedef struct {
  uint32_t *pixels;
  size_t height;
  size_t width;
} PencilCanvas;

typedef struct {
  int x, y, w, h;
} PencilRect;

typedef struct {
  int x, y;
} PencilPoint;

PENCILDEF void pencil_fill(PencilCanvas *canvas, uint32_t color) {
  for (size_t y = 0; y < canvas->height; y++) {
    for (size_t x = 0; x < canvas->width; x++) {
      canvas->pixels[y * canvas->width + x] = color;
    }
  }
}

PENCILDEF void pencil_draw_clear(PencilCanvas *canvas, uint32_t color) {
  for (int i = 0; i < canvas->width * canvas->height; i++)
    canvas->pixels[i] = color;
}

PENCILDEF int rect_create(int x, int y, int w, int h) {
  int x0 = x, y0 = y;
  int x1 = x + w, y1 = y + h;
  if (x0 < x)
    x0 = 0;
  if (y0 < 0)
    y0 = 0;
  if (x1 > w)
    x1 = w;
  if (y1 > h)
    y1 = h;
  return 0;
}

PENCILDEF void pencil_draw_rect(PencilCanvas *canvas, int x, int y, size_t w,
                                size_t h, uint32_t color) {
  for (int dy = 0; dy < (int)h; dy++) {
    int ny = y + dy;
    if (0 <= y && y <= (int)canvas->height) {
      for (int dx = 0; dx < (int)w; dx++) {
        int nx = x + dx;
        if (0 <= x && x <= (int)canvas->width)
          canvas->pixels[ny * canvas->width + nx] = color;
      }
    }
  }
}

PENCILDEF void swap(int *a, int *b) {
  *a = *a ^ *b; // t
  *b = *a ^ *b; // t^b
  *a = *a ^ *b; // t^a
}

PENCILDEF void swap_point(PencilPoint *p1, PencilPoint *p2) {
  swap(&p1->x, &p2->x);
  swap(&p1->y, &p2->y);
}

PENCILDEF bool pencil_point_steep(PencilPoint *p1, PencilPoint *p2) {
  return abs(p1->y - p2->y) > abs(p1->x - p2->x);
}

PENCILDEF void pencil_draw_line(PencilCanvas *canvas, PencilPoint *p1,
                                PencilPoint *p2, uint32_t color) {
  // Bresenham
  LOG_INFO("RAW  p1:{%d,%d} p2:{%d,%d}", p1->x, p1->y, p2->x, p2->y);
  bool steep = pencil_point_steep(p1, p2);
  if (steep) {
    swap(&p1->x, &p1->y);
    swap(&p2->x, &p2->y);
  }

  if (p1->x > p2->x) {
    swap_point(p1, p2);
  }
  int dx = p2->x - p1->x;
  int dy = p2->y - p1->y;

  LOG_INFO("NORM p1: {%d, %d}, p2: {%d, %d}", p1->x, p1->y, p2->x, p2->y);
  int e = 0, y = p1->y;
  if (steep) {
    for (int x = p1->x; x <= p2->x; x++) {
      e += abs(dy);
      if ((e << 1) >= dx) {
        y += dy >= 0 ? 1 : -1;
        e -= dx;
      }

      if (y > 0 && y < canvas->width && x > 0 && x < canvas->height)
        canvas->pixels[x * canvas->width + y] = color;
    }
  } else {
    for (int x = p1->x; x <= p2->x; x++) {
      e += abs(dy);
      if ((e << 1) >= dx) {
        y += dy >= 0 ? 1 : -1;
        e -= dx;
      }

      if (x > 0 && x < canvas->width && y > 0 && y < canvas->height)
        canvas->pixels[y * canvas->width + x] = color;
    }
  }
}

PENCILDEF void pencil_draw_circle(PencilCanvas *canvas, int x, int y, size_t r,
                                  uint32_t color) {
  int x1 = x - (int)r;
  int y1 = y - (int)r;
  int x2 = x + (int)r;
  int y2 = y + (int)r;

  if (x1 < 0 || x2 > canvas->width || y1 < 0 || y2 > canvas->height) {
    fprintf(stderr, "out of range, x1: %d, y1: %d, x2: %d, y2: %d\n", x1, y1,
            x2, y2);
    return;
  }

  for (int i = y1; i < y2; i++) {
    for (int j = x1; j < x2; j++) {
      if (((i - y) * (i - y) + (j - x) * (j - x)) > r * r)
        continue;
      canvas->pixels[i * canvas->width + j] = color;
    }
  }
}

int pencil_save_to_ppm(PencilCanvas *canvas, const char *file_path) {
  int result = 0;
  FILE *f = NULL;

  {
    f = fopen(file_path, "wb");
    if (f == NULL) {
      return_defer(errno);
    }
    fprintf(f, "P6\n%zu %zu 255\n", canvas->width, canvas->height);
    if (ferror(f)) {
      return_defer(errno);
    }

    for (size_t i = 0; i < canvas->width * canvas->height; i++) {
      // 0xAABBGGRR;
      uint32_t pixel = canvas->pixels[i];
      uint8_t bytes[3] = {(pixel >> (8 * 0) & 0xFF), (pixel >> (8 * 1) & 0xFF),
                          (pixel >> (8 * 2) & 0xFF)};
      fwrite(bytes, sizeof(bytes), 1, f);
      if (ferror(f))
        return_defer(errno);
    }
  }

defer:
  if (f)
    fclose(f);
  return result;
}

#endif /* ifndef PENCIL_C_ */
