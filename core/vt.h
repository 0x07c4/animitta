#ifndef CUTERM_CORE_VT_H
#define CUTERM_CORE_VT_H

#include <stddef.h>

#include "screen.h"

typedef enum {
  CT_VT_TEXT,
  CT_VT_ESC,
  CT_VT_CSI,
  CT_VT_OSC,
  CT_VT_OSC_ESC,
} CtVtState;

typedef struct {
  CtVtState state;
  char csi_params[64];
  size_t csi_params_len;
} CtVtParser;

void ct_vt_init(CtVtParser *parser);
void ct_vt_feed(CtVtParser *parser, CtScreen *screen, const char *data, size_t n);

#endif
