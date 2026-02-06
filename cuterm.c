// gui_term.c
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_pixels.h>
#define _GNU_SOURCE
#define LOG_ENABLE
#include "base/log.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <fcntl.h>
#include <poll.h>
#include <pty.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// ---- stb_easy_font: tiny text rendering (no deps) ----
#define STB_EASY_FONT_IMPLEMENTATION
// Minimal stb_easy_font implementation (public domain) by Sean Barrett.
// Source: https://github.com/nothings/stb (we're embedding the needed part)
static int stb_easy_font_width(const char *text) {
  (void)text;
  return 0;
}
static int stb_easy_font_height(const char *text) {
  (void)text;
  return 0;
}
static int stb_easy_font_print(float x, float y, char *text,
                               unsigned char color[4], void *vertex_buffer,
                               int vbuf_size);
// --- begin embedded stb_easy_font.c ---
/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - Public Domain (www.unlicense.org)
ALTERNATIVE B - MIT License
------------------------------------------------------------------------------
*/
#ifndef STB_EASY_FONT_H
#define STB_EASY_FONT_H

static unsigned char stb_easy_font_chardata[96][4] = {
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1},
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1},
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1},
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1},
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1},
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1},
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1},
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1},
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1},
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1},
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1},
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1},
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1},
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1},
};
static int stb_easy_font_print(float x, float y, char *text,
                               unsigned char color[4], void *vertex_buffer,
                               int vbuf_size) {
  (void)color;
  float *v = (float *)vertex_buffer;
  int quads = 0;
  int i = 0;
  float start_x = x;

  // Super minimal: render each non-control char as a 8x12 rectangle
  // outline-ish. This is NOT pretty. It's for bringing the pipeline up, not
  // aesthetics.
  while (text[i] && (quads * 4 * 16) < vbuf_size) {
    unsigned char c = (unsigned char)text[i++];
    if (c == '\n') {
      y += 14;
      x = start_x;
      continue;
    }
    if (c == '\r') {
      x = start_x;
      continue;
    }
    if (c < 32)
      continue;

    float w = 8.0f, h = 12.0f;

    // 2 triangles = quad
    // tri1
    *v++ = x;
    *v++ = y;
    *v++ = x + w;
    *v++ = y;
    *v++ = x + w;
    *v++ = y + h;
    // tri2
    *v++ = x;
    *v++ = y;
    *v++ = x + w;
    *v++ = y + h;
    *v++ = x;
    *v++ = y + h;

    quads++;
    x += 8.0f;
  }
  return quads * 2; // triangles
}
#endif
// --- end embedded stb_easy_font.c ---

extern char **environ;

// ---- simple scrollback buffer ----
#define RING_SIZE (1 << 20) // 1MB output ring
static char ring[RING_SIZE];
static size_t ring_len = 0;

typedef enum { T_TEXT, T_ESC, T_CSI, T_OSC, T_OSC_ESC } ansi_state_t;

static void sanitize_printable(char *dst, size_t dst_sz, const char *src,
                               size_t n) {
  size_t j = 0;
  enum { T_TEXT, T_ESC, T_CSI, T_OSC, T_OSC_ESC } st = T_TEXT;

  for (size_t i = 0; i < n && j + 1 < dst_sz; i++) {
    unsigned char c = (unsigned char)src[i];
    if (st == T_TEXT) {
      if (c == 0x1b) {
        st = T_ESC;
        continue;
      } // ESC
      if (c == '\n' || c == '\t') {
        dst[j++] = (char)c;
        continue;
      }
      if (c < 32 || c == 127)
        continue; // 控制字符(含 BEL)
      dst[j++] = (char)c;
      continue;
    }

    if (st == T_ESC) {
      if (c == '[') {
        st = T_CSI;
        continue;
      } // CSI
      if (c == ']') {
        st = T_OSC;
        continue;
      } // OSC
      st = T_TEXT; // 其他 ESC 序列，直接丢弃
      continue;
    }

    if (st == T_CSI) {
      // CSI 以 0x40-0x7E 的终止字节结束（m, h, l, K 等）
      if (c >= 0x40 && c <= 0x7E)
        st = T_TEXT;
      continue; // 吞掉 CSI 内容
    }

    if (st == T_OSC) {
      // OSC 以 BEL 或 ESC \ 结束
      if (c == 0x07) {
        st = T_TEXT;
        continue;
      }
      if (c == 0x1b) {
        st = T_OSC_ESC;
        continue;
      }
      continue; // 吞掉 OSC 内容
    }

    // T_OSC_ESC
    if (c == '\\')
      st = T_TEXT; // ESC
    else
      st = T_OSC;
  }
  dst[j] = '\0';
}

static void ring_append(const char *data, size_t n) {
  static ansi_state_t st = T_TEXT;
  char safe[512];
  sanitize_printable(safe, sizeof(safe), data, n);

  for (size_t i = 0; i < n; i++) {
    unsigned char c = (unsigned char)data[i];

    // --------- 先吞掉 ANSI/VT 控制序列（真正解决右边“乱码”） ---------
    if (st == T_TEXT) {
      if (c == 0x1b) {
        st = T_ESC;
        continue;
      } // ESC
    } else if (st == T_ESC) {
      if (c == '[') {
        st = T_CSI;
        continue;
      } // CSI
      if (c == ']') {
        st = T_OSC;
        continue;
      } // OSC
      st = T_TEXT; // 其他 ESC 序列，丢掉
      continue;
    } else if (st == T_CSI) {
      // CSI 以 0x40-0x7E 的终止字节结束（m/h/l/K/H 等）
      if (c >= 0x40 && c <= 0x7E)
        st = T_TEXT;
      continue; // CSI 内容全吞
    } else if (st == T_OSC) {
      // OSC 以 BEL(0x07) 或 ESC \ 结束
      if (c == 0x07) {
        st = T_TEXT;
        continue;
      }
      if (c == 0x1b) {
        st = T_OSC_ESC;
        continue;
      }
      continue; // OSC 内容全吞
    } else {    // T_OSC_ESC
      if (c == '\\')
        st = T_TEXT; // ESC \
      else st = T_OSC;
      continue;
    }
    // --------------------------------------------------------------------

    if (c == '\b' || c == 0x7f) {
      if (ring_len > 0) {
        if (ring[ring_len - 1] != '\n') {
          ring_len--;
          ring[ring_len] = '\0';
        }
      }
      continue;
    }
    if (c == '\r') {
      // CR is common in PTY streams (e.g. CRLF). Do not erase the current line.
      continue;
    }
    if (c < 32 && c != '\n' && c != '\t') {
      continue;
    }
    if (ring_len + 1 >= RING_SIZE) {
      memmove(ring, ring + 1, ring_len - 1);
      ring_len--;
    }
    ring[ring_len++] = (char)c;
    ring[ring_len] = '\0';
  }
}

// ---- PTY spawn ----
static pid_t spawn_shell_pty(int *out_master_fd) {
  int master = -1;
  pid_t pid = forkpty(&master, NULL, NULL, NULL);
  if (pid < 0)
    return -1;

  if (pid == 0) {
    char *argv[] = {"bash", "-i", NULL};
    execve("/bin/bash", argv, environ);
    _exit(127);
  }

  // non-blocking read
  int flags = fcntl(master, F_GETFL, 0);
  if (flags >= 0)
    fcntl(master, F_SETFL, flags | O_NONBLOCK);

  *out_master_fd = master;
  return pid;
}

// ---- OpenGL helpers (fixed pipeline, simple) ----
static void gl_setup_ortho(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, h, 0, -1, 1); // y down
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

static void draw_text(float x, float y, const char *text) {
  static float vbuf[1 << 16]; // vertex buffer
  unsigned char color[4] = {255, 255, 255, 255};
  int tris =
      stb_easy_font_print(x, y, (char *)text, color, vbuf, (int)sizeof(vbuf));

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, vbuf);
  glDrawArrays(GL_TRIANGLES, 0, tris * 3);
  glDisableClientState(GL_VERTEX_ARRAY);
}

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

  int win_w = 1000, win_h = 700;
  SDL_Window *win = SDL_CreateWindow(
      "gui-term (milestone-1)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      win_w, win_h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (!win) {
    fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }
  SDL_GLContext ctx = SDL_GL_CreateContext(win);
  if (!ctx) {
    fprintf(stderr, "SDL_GL_CreateContext failed: %s\n", SDL_GetError());
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }
  SDL_Renderer *ren = SDL_CreateRenderer(
      win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!ren) {
    LOG_ERROR("CreateRenderer: %s", SDL_GetError());
    return 1;
  }
  if (TTF_Init() != 0) {
    LOG_ERROR("TTF_Init: %s", SDL_GetError());
    return 1;
  }
  TTF_Font *font =
      TTF_OpenFont("/usr/share/fonts/TTF/JetBrainsMonoNL-Regular.ttf", 28);
  if (!font) {
    LOG_ERROR("OpenFont: %s", SDL_GetError());
    return 1;
  }

  SDL_StartTextInput();

  int master = -1;
  pid_t child = spawn_shell_pty(&master);
  if (child < 0) {
    perror("spawn_shell_pty");
    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  struct pollfd pfd = {.fd = master, .events = POLLIN};

  int running = 1;
  while (running) {
    // 1) pump SDL events
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT)
        running = 0;
      if (e.type == SDL_WINDOWEVENT &&
          e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        win_w = e.window.data1;
        win_h = e.window.data2;
      }
      if (e.type == SDL_TEXTINPUT) {
        // send typed text to PTY
        const char *t = e.text.text;
        LOG_ERROR("[TX] text input: '%s'\n", t);
        if (t && *t)
          (void)write(master, t, strlen(t));
      }
      if (e.type == SDL_KEYDOWN) {
        // minimal keys: Enter, Backspace, Ctrl+C
        SDL_Keycode k = e.key.keysym.sym;
        SDL_Keymod mod = SDL_GetModState();
        if (k == SDLK_RETURN) {
          LOG_ERROR("[TX] enter\n");
          char c = '\r';
          (void)write(master, &c, 1);
        } else if (k == SDLK_BACKSPACE) {
          char c = 0x7f; // DEL
          (void)write(master, &c, 1);
        } else if (k == SDLK_c && (mod & KMOD_CTRL)) {
          char c = 0x03; // ETX
          (void)write(master, &c, 1);
        }
      }
    }

    // 2) read PTY output (non-blocking + poll)
    int pr = poll(&pfd, 1, 0);
    if (pr > 0) {
      if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
        fprintf(stderr, "[RX] poll revents=0x%x (err/hup/nval)\n", pfd.revents);
      }
      if (pfd.revents & POLLIN) {
        char buf[4096];
        for (;;) {
          ssize_t n = read(master, buf, sizeof(buf));
          if (n > 0)
            ring_append(buf, (size_t)n);
          else
            break;
        }
      }
    }

    // 3) render
    // glClearColor(0.f, 0.f, 0.f, 1.f);
    // glClear(GL_COLOR_BUFFER_BIT);
    // gl_setup_ortho(win_w, win_h);
    //
    // // draw the last chunk of ring buffer (super naive: show tail)
    // const size_t MAX_SHOW = 20000; // characters
    // const char *text = ring;
    // if (ring_len > MAX_SHOW)
    //   text = ring + (ring_len - MAX_SHOW);
    //
    // glColor3f(0.9f, 0.9f, 0.9f);
    // draw_text(10.f, 20.f, text);
    //
    // SDL_GL_SwapWindow(win);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    const int TOP_MARGIN = 10;
    const int BOTTOM_MARGIN = 10;
    const int MAX_VISIBLE_LINES = 256;
    int line_h = TTF_FontLineSkip(font);
    if (line_h <= 0)
      line_h = TTF_FontHeight(font);
    if (line_h <= 0)
      line_h = 18;
    int max_lines = (win_h - TOP_MARGIN - BOTTOM_MARGIN) / line_h;
    if (max_lines < 1)
      max_lines = 1;
    if (max_lines > MAX_VISIBLE_LINES)
      max_lines = MAX_VISIBLE_LINES;

    const char *start = ring;
    if (ring_len > 30000) {
      start = ring + (ring_len - 30000);
    }

    const char *lines[MAX_VISIBLE_LINES];
    int count = 0;
    lines[count++] = start;
    for (const char *p = start; *p; p++) {
      if (*p != '\n')
        continue;
      const char *next = p + 1;
      if (*next == '\0')
        continue;
      if (count < max_lines) {
        lines[count++] = next;
      } else {
        memmove(lines, lines + 1, (size_t)(max_lines - 1) * sizeof(lines[0]));
        lines[max_lines - 1] = next;
      }
    }

    int y = TOP_MARGIN;
    SDL_Color fg = {230, 230, 230, 255};

    for (int i = 0; i < count; i++) {
      const char *line = lines[i];
      const char *end = line;
      while (*end && *end != '\n' && *end != '\r')
        end++;

      int len = (int)(end - line);
      if (len <= 0) {
        y += line_h;
        continue;
      }

      char tmp[2048];
      if (len >= (int)sizeof(tmp))
        len = (int)sizeof(tmp) - 1;
      memcpy(tmp, line, (size_t)len);
      tmp[len] = '\0';

      SDL_Surface *surf = TTF_RenderUTF8_Blended(font, tmp, fg);
      if (surf) {
        SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
        SDL_Rect dst = {10, y, surf->w, surf->h};
        SDL_FreeSurface(surf);
        if (tex) {
          SDL_RenderCopy(ren, tex, NULL, &dst);
          SDL_DestroyTexture(tex);
        }
      }

      y += line_h;
    }
    SDL_RenderPresent(ren);

    // 60-ish fps
    SDL_Delay(16);
  }

  // cleanup
  close(master);
  int status = 0;
  (void)waitpid(child, &status, 0);

  SDL_StopTextInput();
  SDL_GL_DeleteContext(ctx);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return 0;
}
