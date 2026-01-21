#include "base/geo.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

int main(void) {
  const int W = 800;
  const int H = 450;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window *win =
      SDL_CreateWindow("C Graphics (CPU framebuffer)", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_SHOWN);
  if (!win) {
    fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  fprintf(stdout, "w:%d h:%d\n", w, h);

  SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  if (!ren) {
    fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  SDL_RendererInfo rendererInfo;
  if (SDL_GetRendererInfo(ren, &rendererInfo)) {
    fprintf(stdout, "%s\n", rendererInfo.name);
  }

  uint32_t *pixels = malloc((size_t)W * (size_t)H * sizeof(uint32_t));
  if (!pixels) {
    fprintf(stderr, "malloc failed\n");
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888,
                                       SDL_TEXTUREACCESS_STREAMING, W, H);
  if (!tex) {
    fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
    free(pixels);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  uint32_t start_ticks = SDL_GetTicks();
  int running = 1;

  fprintf(stdout, "start_ticks: %d\n", start_ticks);

  while (running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT)
        running = 0;
      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
        running = 0;
    }

    uint32_t t = SDL_GetTicks() - start_ticks;

    fprintf(stdout, "t: %d\n", t);

    pencil_clear(pixels, W, H, 0xFF101018u);
    draw_line(pixels, W, H, 10, 10, W - 10, 10, 0xFFFF0000u);
    draw_line(pixels, W, H, 10, 10, 10, H - 10, 0xFF00FF00u);
    draw_line(pixels, W, H, 10, 10, W - 10, H - 10, 0xFF00A0FFu);

    int rect_w = 120;
    int rect_h = 80;
    int x = (int)((t / 4) % (uint32_t)(W + rect_w)) - rect_w;
    int y = H / 2 - rect_h / 2;

    draw_rect_filled(pixels, W, H, x, y, rect_w, rect_h, 0xFFFFCC00u);

    SDL_UpdateTexture(tex, NULL, pixels, W * (int)sizeof(uint32_t));
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, tex, NULL, NULL);
    SDL_RenderPresent(ren);
  }

  SDL_DestroyTexture(tex);
  free(pixels);
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return 0;
}
