#include <grrlib.h>

#include <wiiuse/wpad.h>

#include <stdio.h>
#include <stdlib.h>
#include <mruby.h>
#include <mruby/irep.h>
#include <mruby/string.h>
#include <mruby/array.h>
#include "mrbwii.h"
#include "font_png.h"

#define GRRLIB_WHITE   0xFFFFFFFF
#define GRRLIB_BLACK   0x3333FFFF

extern const uint8_t program[];

GRRLIB_texImg *tex_font;
int main(int argc, char **argv) {
  GRRLIB_Init();
  tex_font = GRRLIB_LoadTexture(font_png);
  GRRLIB_InitTileSet(tex_font, 8, 16, 0);

  // pad initialisation
  WPAD_Init();
  WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

  while(1) {
    GRRLIB_FillScreen(GRRLIB_BLACK);
    GRRLIB_Printf(5, 25, tex_font, GRRLIB_WHITE, 1, "%s", "Hello, world");
    GRRLIB_Render();
    VIDEO_WaitVSync();
  }

  return 0;
}
