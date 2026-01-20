#include "pencil.c"
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define WIDTH 800
#define HEIGHT 600

#define COLS 8
#define ROWS 6
#define CELL_WIDTH (WIDTH / COLS)
#define CELL_HEIGHT (HEIGHT / ROWS)

static uint32_t pixels[HEIGHT * WIDTH] = {0};

bool chess_example() {
  PencilCanvas canvas = {.pixels = pixels, .height = HEIGHT, .width = WIDTH};
  pencil_fill(&canvas, PENCIL_GRAY);
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLS; x++) {
      uint32_t color;
      if ((x + y) % 2 == 0) {
        color = PENCIL_RED;
      } else {
        color = PENCIL_GREEN;
      }
      pencil_rect(&canvas, x * CELL_WIDTH, y * CELL_HEIGHT, CELL_WIDTH,
                  CELL_HEIGHT, color);
    }
  }
  const char *output = "chess.ppm";
  int err = pencil_save_to_ppm(&canvas, output);
  if (err) {
    fprintf(stderr, "ERROR: could not save file %s: %s\n", output,
            strerror(errno));
    return false;
  }
  return true;
}

bool circle_example(void) {
  PencilCanvas canvas = {.pixels = pixels, .height = HEIGHT, .width = WIDTH};
  pencil_fill(&canvas, PENCIL_GRAY);

  size_t radius = CELL_WIDTH < CELL_HEIGHT ? CELL_WIDTH : CELL_HEIGHT;
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLS; x++) {
      uint32_t color;
      if ((x + y) % 2 == 0) {
        color = PENCIL_RED;
      } else {
        color = PENCIL_GREEN;
      }
      pencil_circle(&canvas, x * CELL_WIDTH + CELL_WIDTH / 2,
                    y * CELL_HEIGHT + CELL_HEIGHT / 2, radius / 2, color);
    }
  }
  const char *output = "circle.ppm";
  int err = pencil_save_to_ppm(&canvas, output);
  if (err) {
    fprintf(stderr, "ERROR: could not save file %s: %s\n", output,
            strerror(errno));
    return false;
  }
  return true;
}

int main() {
  if (!chess_example())
    return -1;
  if (!circle_example())
    return -1;
  return 0;
}
