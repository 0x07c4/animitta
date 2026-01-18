#include "pencil.c"
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define WIDTH 800
#define HEIGHT 600

static uint32_t pixels[HEIGHT][WIDTH] = {0};

int main() {
  PencilCanvas canvas = {.pixels = pixels, .height = HEIGHT, .width = WIDTH};
  pencil_fill(&canvas, 0xFF202020);
  pencil_rect(&canvas, WIDTH / 2, HEIGHT / 2, 50, 30, 0xFF2020FF);
  const char *output = "output.ppm";
  int err = pencil_save_to_ppm(&canvas, output);
  if (err) {
    fprintf(stderr, "ERROR: could not save file %s: $s\n", output,
            strerror(errno));
    return 1;
  }
  return 0;
}
