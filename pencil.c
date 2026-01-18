#ifndef PENCIL_C_
#define PENCIL_C_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define PENCILDEF static inline

typedef struct {
  uint32_t *pixels;
  size_t height;
  size_t width;
} PencilCanvas;

typedef struct {
  int x, y, w, h;
} PencilRect;

PENCILDEF void pencil_fill(PencilCanvas *canvas, uint32_t color) {
  for (size_t y = 0; y < canvas->height; y++) {
    for (size_t x = 0; x < canvas->width; x++) {
      canvas->pixels[y * canvas->width + x] = color;
    }
  }
}

PENCILDEF void pencil_clear(PencilCanvas *canvas, uint32_t color) {
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

PENCILDEF void pencil_rect(PencilCanvas *canvas, int x, int y, int w, int h,
                           uint32_t color) {

  if (rect_create(x, y, w, h))
    return;
  for (int yy = y0; yy < y1; yy++) {
    for (int xx = x0; xx < x1; xx++) {
      pencil_fill(canvas->pixels, w, h, xx, yy, color);
    }
  }
}

PENCILDEF void draw_line(uint32_t *pixels, int w, int h, int x1, int y1, int x2,
                         int y2, uint32_t color) {
  int dx = x2 - x1;
  int dy = y2 - y1;

  float dl = 0.001;
  for (float i = 0.0; i <= 1.0; i += dl) {
    int tx = x1 + dx * i;
    int ty = y1 + dy * i;
    // put_pixel(pixels, x2, y2, i, ty, color);
    if (tx > w || ty > h)
      break;
    pixels[ty * w + tx] = color;
    fprintf(stdout, "tx: %d, ty: %d, color: %x\n", tx, ty, color);
  }
}
#endif /* ifndef PENCIL_C_ */
