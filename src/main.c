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

// #define GRRLIB_WHITE   0xFFFFFFFF
// #define GRRLIB_BLACK   0x3333FFFF

extern const uint8_t program[];

GRRLIB_texImg *tex_font;
int main(int argc, char **argv) {
  GRRLIB_Init();
  tex_font = GRRLIB_LoadTexture(font_png);
  GRRLIB_InitTileSet(tex_font, 8, 16, 0);

  // console allows usage of printf
//  void* frameBuffer = (void*)xfb[0];
//  console_init(frameBuffer,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
//  VIDEO_Configure(rmode);
//  VIDEO_SetNextFramebuffer(frameBuffer);
//  VIDEO_SetBlack(FALSE);
//  VIDEO_Flush();
//  VIDEO_WaitVSync();

  // pad initialisation
  WPAD_Init();
  WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

//  while(1) {
//    VIDEO_WaitVSync();
//    GRRLIB_FillScreen(GRRLIB_BLACK);
//    GRRLIB_Printf(5, 25, tex_font, GRRLIB_WHITE, 1, "%s", "Hello, world");
//    GRRLIB_Render();
//  }
//  printf("\x1b[10;0H");
//  printf("what's going on?\n");

  mrb_state *mrb = mrb_open();
  if (!mrb) { return 1; }
  struct RClass *dwii_module = mrb_define_module(mrb, "DreamPresentWii");
  define_module_functions(mrb, dwii_module);
printf("after define_module_functions xfb: %lx\n", (unsigned long)xfb);

  mrb_load_irep(mrb, program);

  return 0;
}
