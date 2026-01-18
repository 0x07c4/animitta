#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PENCILDEF static inline

PENCILDEF void put_pixel(uint32_t *pixels, int w, int h, int x, int y,
                         uint32_t color) {
  if ((unsigned)x >= (unsigned)w || (unsigned)y >= (unsigned)h)
    return;
  pixels[y * w + x] = color;
}

PENCILDEF void pencil_clear(uint32_t *pixels, int w, int h, uint32_t color) {
  for (int i = 0; i < w * h; i++)
    pixels[i] = color;
}

static void draw_rect_filled(uint32_t *pixels, int W, int H, int x, int y,
                             int w, int h, uint32_t color) {
  int x0 = x, y0 = y;
  int x1 = x + w, y1 = y + h;
  if (x0 < x)
    x0 = 0;
  if (y0 < 0)
    y0 = 0;
  if (x1 > W)
    x1 = W;
  if (y1 > H)
    y1 = H;

  for (int yy = y0; yy < y1; yy++) {
    for (int xx = x0; xx < x1; xx++) {
      put_pixel(pixels, W, H, xx, yy, color);
    }
  }
}

static void draw_line(uint32_t *pixels, int w, int h, int x1, int y1, int x2,
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
