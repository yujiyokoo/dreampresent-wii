#include <grrlib.h>

#include <wiiuse/wpad.h>
#include <ogc/system.h>
#include <ogc/lwp_watchdog.h>
#include <asndlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

  // console allows usage of printf, but may cause issues with terminal output on the emulator
  // void* frameBuffer = (void*)xfb[0];
  // console_init(frameBuffer,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
  SYS_STDIO_Report(true);
	// Output directly to Dolphin emulated UART
	SYS_Report("Hello UART\r");

	// Redirect stderr to UART
	SYS_STDIO_Report(false);

	fprintf(stderr,"stderr: Hello stderr UART!\r");

	// Redirect both stdio and stderr to UART
	SYS_STDIO_Report(true);

	fprintf(stderr,"stderr: Hello stderr UART!\r");
	fprintf(stdout,"stdout: Hello stdout UART!\r");
	printf("stdout: Hello printf UART!\r");
	printf("stdout: Hello printf UART again!\r");
  printf("stdout: Hello printf!\r");
  SYS_Report("Hello from OSReport!\r");

//  VIDEO_Configure(rmode);
//  VIDEO_SetNextFramebuffer(frameBuffer);
//  VIDEO_SetBlack(FALSE);
//  VIDEO_Flush();
//  VIDEO_WaitVSync();

  // pad initialisation
  PAD_Init();

  // Wii Remote
  WPAD_Init();
  WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
  WPAD_SetVRes(0,640,480);

  // TODO: call from some other init?
  printf("initializing controller reader\r");
  init_controller_reader();
  printf("done initializing controller reader\r");

  mrb_state *mrb = mrb_open();
  if (!mrb) { return 1; }
  struct RClass *dwii_module = mrb_define_module(mrb, "DreamPresentWii");
  define_module_functions(mrb, dwii_module);
  dream_present_png_init(mrb);

  mrb_load_irep(mrb, program);

  return 0;
}
