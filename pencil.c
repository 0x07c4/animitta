#ifndef PENCIL_C_
#define PENCIL_C_

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define PENCILDEF static inline

#define PENCIL_GRAY 0xFF202020
#define PENCIL_RED 0xFF0000FF
#define PENCIL_GREEN 0xFF00FF00

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

PENCILDEF void pencil_rect(PencilCanvas *canvas, int x, int y, size_t w,
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

PENCILDEF void pencil_circle(PencilCanvas *canvas, int x, int y, size_t r,
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
