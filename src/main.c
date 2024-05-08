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

const int check_duration = 15;
int has_below_threshold(gforce_t* ghistory, int index, float threshold) {
  int start = (index + 60 - check_duration) % 60;
  for (int i = 0; i < check_duration; i++) {
    if (ghistory[(start + i) % 60].x < threshold) {
      return 1;
    }
  }
  return 0;
}

int has_above_threshold(gforce_t* ghistory, int index, float threshold) {
  int start = (index + 60 - check_duration) % 60;
  for (int i = 0; i < check_duration; i++) {
    if (ghistory[(start + i) % 60].x > threshold) {
      return 1;
    }
  }
  return 0;
}

// attempt to reduce jittering
int no_recent_occurence(int8_t* neg_pos_history, int index, int8_t target) {
  int start = (index + 60 - check_duration) % 60;
  for (int i = 0; i < check_duration; i++) {
    if (neg_pos_history[(start + i) % 60] == target) {
      return 0;
    }
  }
  return 1;
}

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

  gforce_t gforce;
  int8_t i = 0;
  gforce_t ghistory[60];
  gforce_t max = {0.0, 0.0, 0.0};
  gforce_t min = {0.0, 0.0, 0.0};
  int8_t neg_pos_history[60] = {0};
  while(0) {
    VIDEO_WaitVSync();
    WPAD_ScanPads();
    WPAD_GForce(0, &(ghistory[i]));
    gforce = ghistory[i % 60];
    max.x = max.x > gforce.x ? max.x : gforce.x;
    max.y = max.y > gforce.y ? max.y : gforce.y;
    max.z = max.z > gforce.z ? max.z : gforce.z;
    min.x = min.x < gforce.x ? min.x : gforce.x;
    min.y = min.y < gforce.y ? min.y : gforce.y;
    min.z = min.z < gforce.z ? min.z : gforce.z;
    GRRLIB_Printf(5, 25, tex_font, GRRLIB_WHITE, 1, "Gforce x: %f, y: %f, z: %f", gforce.x, gforce.y, gforce.z);
    GRRLIB_Printf(5, 55, tex_font, GRRLIB_WHITE, 1, "max x: %f, y: %f, z: %f", max.x, max.y, max.z);
    GRRLIB_Printf(5, 85, tex_font, GRRLIB_WHITE, 1, "min x: %f, y: %f, z: %f", min.x, min.y, min.z);

    char* neg_pos = "";
    neg_pos_history[i] = 0; // should use if-else-if-else but this is good enough
    // if it's near 0, and it's been positive recently, and been negative before that, then it's a neg_pos
    if(abs(gforce.x) < 0.5 && has_above_threshold(ghistory, i, 1.0) && has_below_threshold(ghistory, (i+45)%60, -2.0)) {
      neg_pos_history[i] = -1;
      if (no_recent_occurence(neg_pos_history, i, 1) && no_recent_occurence(neg_pos_history, i, -1)) {
        neg_pos = "neg_pos detected\n";
      }
    }
    char* pos_neg = "";
    // if it's near 0, and it's been negative recently, and been positive before that, then it's a pos_neg
    if(abs(gforce.x) < 0.5 && has_below_threshold(ghistory, i, -1.0) && has_above_threshold(ghistory, (i+45)%60, 2.0)) {
      neg_pos_history[i] = 1;
      if (no_recent_occurence(neg_pos_history, i, -1) && no_recent_occurence(neg_pos_history, i, 1)) {
        pos_neg = "pos_neg detected\n";
      }
    }
    GRRLIB_Printf(5, 115, tex_font, GRRLIB_WHITE, 1, "%s", neg_pos);
    GRRLIB_Printf(5, 145, tex_font, GRRLIB_WHITE, 1, "%s", pos_neg);

    GRRLIB_Render();

    i = (i+1) % 60;
  }
//  printf("\x1b[10;0H");
//  printf("what's going on?\n");

  // todo: call from some other init?
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
