#include "vt.h"

static int ct_vt_parse_first_param(const char *buf, size_t len,
                                   int default_value) {
  size_t i = 0;
  int value = 0;
  int has_digits = 0;

  if (i < len &&
      (buf[i] == '?' || buf[i] == '>' || buf[i] == '<' || buf[i] == '=')) {
    i++;
  }
  for (; i < len; i++) {
    unsigned char c = (unsigned char)buf[i];
    if (c >= '0' && c <= '9') {
      value = value * 10 + (int)(c - '0');
      has_digits = 1;
      continue;
    }
    if (c == ';') {
      break;
    }
    break;
  }
  if (!has_digits) {
    return default_value;
  }
  return value;
}

static int ct_vt_parse_private_mode(const char *buf, size_t len) {
  if (len == 0 || buf[0] != '?') {
    return -1;
  }
  return ct_vt_parse_first_param(buf, len, -1);
}

static void ct_vt_handle_csi(CtScreen *screen, char final, const char *params,
                             size_t params_len) {
  if (final == 'J') {
    int mode = ct_vt_parse_first_param(params, params_len, 0);
    if (mode == 2 || mode == 3) {
      ct_screen_clear(screen);
    }
    return;
  }

  if (final == 'h' || final == 'l') {
    int mode = ct_vt_parse_private_mode(params, params_len);
    if (mode == 47 || mode == 1047 || mode == 1049) {
      ct_screen_clear(screen);
    }
  }
}

void ct_vt_init(CtVtParser *parser) {
  if (!parser) {
    return;
  }
  parser->state = CT_VT_TEXT;
  parser->csi_params_len = 0;
}

void ct_vt_feed(CtVtParser *parser, CtScreen *screen, const char *data,
                size_t n) {
  size_t i = 0;

  if (!parser || !screen || !data || n == 0) {
    return;
  }

  for (i = 0; i < n; i++) {
    unsigned char c = (unsigned char)data[i];

    if (parser->state == CT_VT_TEXT) {
      if (c == 0x1b) {
        parser->state = CT_VT_ESC;
        continue;
      }
    } else if (parser->state == CT_VT_ESC) {
      if (c == '[') {
        parser->state = CT_VT_CSI;
        parser->csi_params_len = 0;
        continue;
      }
      if (c == ']') {
        parser->state = CT_VT_OSC;
        continue;
      }
      if (c == 'c') {
        ct_screen_clear(screen);
        parser->state = CT_VT_TEXT;
        continue;
      }
      parser->state = CT_VT_TEXT;
      continue;
    } else if (parser->state == CT_VT_CSI) {
      if (c >= 0x40 && c <= 0x7E) {
        ct_vt_handle_csi(screen, (char)c, parser->csi_params,
                         parser->csi_params_len);
        parser->csi_params_len = 0;
        parser->state = CT_VT_TEXT;
      } else if (parser->csi_params_len + 1 < sizeof(parser->csi_params)) {
        parser->csi_params[parser->csi_params_len++] = (char)c;
      }
      continue;
    } else if (parser->state == CT_VT_OSC) {
      if (c == 0x07) {
        parser->state = CT_VT_TEXT;
        continue;
      }
      if (c == 0x1b) {
        parser->state = CT_VT_OSC_ESC;
        continue;
      }
      continue;
    } else {
      if (c == '\\') {
        parser->state = CT_VT_TEXT;
      } else {
        parser->state = CT_VT_OSC;
      }
      continue;
    }

    if (c == '\b' || c == 0x7f) {
      ct_screen_backspace(screen);
      continue;
    }
    if (c == '\r') {
      continue;
    }
    if (c < 32 && c != '\n' && c != '\t') {
      continue;
    }
    ct_screen_push(screen, (char)c);
  }
}
