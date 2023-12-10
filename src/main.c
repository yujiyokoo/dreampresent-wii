#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <mruby.h>
#include <mruby/irep.h>
#include <mruby/string.h>
#include <mruby/array.h>
#include "mrbwii.h"
#include "font_png.h"
#include <grrlib.h>

extern const uint8_t program[];

GRRLIB_texImg *tex_font;
int main(int argc, char **argv) {
  //WPAD_Init();
  PAD_Init();

  GRRLIB_Init();

  tex_font = GRRLIB_LoadTexture(font_png);
	GRRLIB_InitTileSet(tex_font, 8, 16, 0);
  mrb_state *mrb = mrb_open();
  if (!mrb) { return 1; }
  struct RClass *dwii_module = mrb_define_module(mrb, "DreamPresentWii");
  define_module_functions(mrb, dwii_module);

  mrb_load_irep(mrb, program);

	GRRLIB_FillScreen(0xFF0000FF);
  GRRLIB_Render();

  GRRLIB_Exit();

	return 0;
}
