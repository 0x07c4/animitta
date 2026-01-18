// #include "pencil.c"
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 600

#define return_defer(value)                                                    \
  do {                                                                         \
    result = value;                                                            \
    goto defer;                                                                \
  } while (0)

static uint32_t pixels[HEIGHT][WIDTH] = {0};

static void pencil_fill(uint32_t *pixels, size_t width, size_t height,
                        uint32_t color) {
  for (size_t y = 0; y < height; ++y) {
    for (size_t x = 0; x < width; ++x)
      pixels[y * width + x] = color;
  }
}

int pencil_save_to_ppm(uint32_t *pixels, size_t width, size_t height,
                       const char *file_path) {
  int result = 0;
  FILE *f = NULL;

  {
    f = fopen(file_path, "wb");
    if (f == NULL) {
      return_defer(errno);
    }
    fprintf(f, "P6\n%zu %zu 255\n", width, height);
    if (ferror(f)) {
      return_defer(errno);
    }

    for (size_t i = 0; i < width * height; i++) {
      // 0xAABBGGRR;
      uint32_t pixel = pixels[i];
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

int main() {
  pencil_fill(pixels, WIDTH, HEIGHT, 0xFF0000FF);
  const char *output = "output.ppm";
  pencil_save_to_ppm(pixels, WIDTH, HEIGHT, output);

  return 0;
}
