#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <mruby.h>
#include <mruby/irep.h>
#include <mruby/string.h>
#include <mruby/array.h>

#define BUFSIZE 100
extern u32 *xfb ;

struct InputBuf {
  uint16_t buffer[BUFSIZE];
  uint32_t index;
} input_buf;

static mrb_value btn_mrb_buffer;

extern const uint8_t program[];

static mrb_value print_msg(mrb_state *mrb, mrb_value self) {
  char *unwrapped_content;
  mrb_value str_content;

  mrb_get_args(mrb, "S", &str_content);
  unwrapped_content = mrb_str_to_cstr(mrb, str_content);
  //printf("\x1b[0;0H");
  printf("%s\n", unwrapped_content);

  return mrb_nil_value();
}

static mrb_value content_string(mrb_state *mrb, mrb_value self) {
  return mrb_str_new_cstr(mrb,
"\
=\n\
-bkg: /rd/bg_dark.png\n\
-txt,60,30:Developing your Dreamcast apps\n\
                        and games with mruby\n\
-txt,60,120:Yuji Yokoo - @yujiyokoo\n\
-txt,120,260:PRESS START!\n\
\n\
= Resetting timer internally...\n\
-resettimer\n\
\n\
= Table of Contents\n\
-txt,40,40: - About me\n\
 - About this app\n\
 - What is Dreamcast?\n\
 - What is mruby?\n\
 - What is KallistiOS?\n\
 - Homebrew Development Steps\n\
 - Presentation app code overview\n\
 - Running the code\n\
 - Getting the program to the unit\n\
 - Thanks and info\n\
 - Demo\n\
\n\
= About me\n\
-txt,60,30:Yuji Yokoo\n\
  Web dev. Loves Ruby.\n\
  Formerly: Desktop application\n\
    developer (MFC/Win32)\n\
  From: Tokyo, Japan\n\
  Lives in: Adelaide, South Australia\n\
-img,60,260: /rd/yuji_avatar.png\n\
\n\
"
  );
}

static mrb_value get_button_masks(mrb_state *mrb, mrb_value self) {
  mrb_value mask_array;
  mask_array = mrb_ary_new(mrb);

  mrb_ary_push(mrb, mask_array, mrb_fixnum_value(PAD_BUTTON_START));
  mrb_ary_push(mrb, mask_array, mrb_fixnum_value(PAD_BUTTON_LEFT));
  mrb_ary_push(mrb, mask_array, mrb_fixnum_value(PAD_BUTTON_RIGHT));
  mrb_ary_push(mrb, mask_array, mrb_fixnum_value(PAD_BUTTON_UP));
  mrb_ary_push(mrb, mask_array, mrb_fixnum_value(PAD_BUTTON_DOWN));
  mrb_ary_push(mrb, mask_array, mrb_fixnum_value(PAD_BUTTON_A));
  mrb_ary_push(mrb, mask_array, mrb_fixnum_value(PAD_BUTTON_B));

  return mask_array;
}

static u32 CvtRGB (u8 r1, u8 g1, u8 b1, u8 r2, u8 g2, u8 b2)
{
	int y1, cb1, cr1, y2, cb2, cr2, cb, cr;

	y1 = (299 * r1 + 587 * g1 + 114 * b1) / 1000;
	cb1 = (-16874 * r1 - 33126 * g1 + 50000 * b1 + 12800000) / 100000;
	cr1 = (50000 * r1 - 41869 * g1 - 8131 * b1 + 12800000) / 100000;

	y2 = (299 * r2 + 587 * g2 + 114 * b2) / 1000;
	cb2 = (-16874 * r2 - 33126 * g2 + 50000 * b2 + 12800000) / 100000;
	cr2 = (50000 * r2 - 41869 * g2 - 8131 * b2 + 12800000) / 100000;

	cb = (cb1 + cb2) >> 1;
	cr = (cr1 + cr2) >> 1;

	return (y1 << 24) | (cb << 16) | (y2 << 8) | cr;
}

static u32 PACK_PIXEL(int r, int g, int b) {
  return CvtRGB(r, g, b, r, g, b);
}

static void draw_square(int x, int y, int r, int g, int b) {
  int i = 0, j = 0;
//printf("xfb is: %lu\n", (unsigned long)xfb);

  if(r == 0 && g == 0 && b == 0) {
    for(i = 0; i < 20; i++) {
      for(j = 0; j < 10; j++) {
        xfb[x+j + (y+i) * 320] = PACK_PIXEL(r, g, b);
      }
    }
  } else {
//    for(i = 0; i < 20; i++) {
//      for(j = 0; j < 10; j++) {
//        xfb[x+j + (y+i) * 320] = PACK_PIXEL(r, g, b);
//      }
//    }
    int r_light = (r+128 <= 255) ? r+128 : 255;
    int g_light = (g+128 <= 255) ? g+128 : 255;
    int b_light = (b+128 <= 255) ? b+128 : 255;

    int r_dark = (r-64 >= 0) ? r-64 : 0;
    int g_dark = (g-64 >= 0) ? g-64 : 0;
    int b_dark = (b-64 >= 0) ? b-64 : 0;

    // TODO: implement lines and use them.
    for(j = 0; j < 10; j++) {
      xfb[x+j + (y) * 320] = PACK_PIXEL(30, 30, 30);
      xfb[x+j + (y+19) * 320] = PACK_PIXEL(30, 30, 30);
    }
    for(j = 1; j < 9; j++) {
      xfb[x+j + (y+1) * 320] = PACK_PIXEL(r_light, g_light, b_light);
    }
    for(j = 2; j < 10; j++) {
      xfb[x+j + (y+19) * 320] = PACK_PIXEL(r_dark, g_dark, b_dark);
    }
    for(i = 2; i < 19; i++) {
      xfb[x + (y+i) * 320] = PACK_PIXEL(30, 30, 30);
      xfb[x+1 + (y+i) * 320] = PACK_PIXEL(r_light, g_light, b_light);
      for(j = 2; j < 9; j++) {
        xfb[x+j + (y+i) * 320] = PACK_PIXEL(r, g, b);
      }
      xfb[x+9 + (y+i) * 320] = PACK_PIXEL(r_dark, g_dark, b_dark);
    }
  }

}

static mrb_value draw20x20_640(mrb_state *mrb, mrb_value self) {
  mrb_int x, y, r, g, b;
  mrb_get_args(mrb, "iiiii", &x, &y, &r, &g, &b);

  draw_square(x, y, r, g, b);
  return mrb_nil_value();
}

static mrb_value init_controller_buffer(mrb_state *mrb, mrb_value self) {
  btn_mrb_buffer = mrb_ary_new(mrb);;
  input_buf.index = 0;

  int i = 0;
  while(i < BUFSIZE) {
    mrb_ary_set(mrb, btn_mrb_buffer, i, mrb_nil_value());
    input_buf.buffer[i] = 0; i ++ ;
  }

  return mrb_nil_value();
}

void *read_buttons() {
  while(1) {
    input_buf.index = (input_buf.index + 1) % BUFSIZE;
		PAD_ScanPads();
		u16 btns = PAD_ButtonsHeld(0);
		input_buf.buffer[input_buf.index] = btns;

		LWP_YieldThread();
  }
}

static mrb_value start_controller_reader(mrb_state *mrb, mrb_value self) {
  // unimplemented
  lwp_t thread;
  LWP_CreateThread(&thread, read_buttons, NULL, NULL, 0, 0);
  return mrb_fixnum_value(0);
}

static mrb_value get_button_state(mrb_state *mrb, mrb_value self) {
  PAD_ScanPads();
	u16 btns = PAD_ButtonsHeld(0);
  return mrb_fixnum_value(btns);
}

static mrb_value get_button_states(mrb_state *mrb, mrb_value self) {
  // unimplemented
  return mrb_fixnum_value(0);
}

static mrb_value start_btn(mrb_state *mrb, mrb_value self) {
  mrb_int state;
  mrb_get_args(mrb, "i", &state);

  return mrb_bool_value(state & PAD_BUTTON_START);
}

static mrb_value dpad_left(mrb_state *mrb, mrb_value self) {
  mrb_int state;
  mrb_get_args(mrb, "i", &state);

  return mrb_bool_value(state & PAD_BUTTON_LEFT);
}

static mrb_value dpad_right(mrb_state *mrb, mrb_value self) {
  mrb_int state;
  mrb_get_args(mrb, "i", &state);

  return mrb_bool_value(state & PAD_BUTTON_RIGHT);
}

static mrb_value dpad_up(mrb_state *mrb, mrb_value self) {
  // unimplemented
  return mrb_fixnum_value(0);
}

static mrb_value dpad_down(mrb_state *mrb, mrb_value self) {
  mrb_int state;
  mrb_get_args(mrb, "i", &state);

  return mrb_bool_value(state & PAD_BUTTON_DOWN);
}

static mrb_value btn_a(mrb_state *mrb, mrb_value self) {
  mrb_int state;
  mrb_get_args(mrb, "i", &state);

  return mrb_bool_value(state & PAD_BUTTON_A);
}

static mrb_value btn_b(mrb_state *mrb, mrb_value self) {
  mrb_int state;
  mrb_get_args(mrb, "i", &state);

  return mrb_bool_value(state & PAD_BUTTON_B);
}

static mrb_value btn_start(mrb_state *mrb, mrb_value self) {
  mrb_int state;
  mrb_get_args(mrb, "i", &state);

  return mrb_bool_value(state & PAD_BUTTON_START);
}

static mrb_value get_current_button_index(mrb_state *mrb, mrb_value self) {
  // unimplemented
  return mrb_fixnum_value(0);
}

static mrb_value waitvbl(mrb_state *mrb, mrb_value self) {
  VIDEO_WaitVSync();
  return mrb_nil_value();
}

static mrb_value get_next_button_state(mrb_state *mrb, mrb_value self) {
  mrb_int wanted_index;
  mrb_get_args(mrb, "i", &wanted_index);
  int curr_index = input_buf.index;

  if(wanted_index >= BUFSIZE || wanted_index < 0) { wanted_index = wanted_index % BUFSIZE; }

  if(wanted_index == (curr_index + 1) % BUFSIZE) {
    return mrb_nil_value();
  } else {
    return mrb_fixnum_value(input_buf.buffer[wanted_index]);
  }
}

static mrb_value render_png(mrb_state *mrb, mrb_value self) {
  // unimplemented
  return mrb_nil_value();
}

static mrb_value draw_str(mrb_state *mrb, mrb_value self) {
  char *unwrapped_content;
  mrb_value str_content;
  mrb_int x, y, r, g, b, bg_on;

  mrb_get_args(mrb, "Siiiiii", &str_content, &x, &y, &r, &g, &b, &bg_on);
  unwrapped_content = mrb_str_to_cstr(mrb, str_content);
  // TODO: get coordinates working
  printf("%s\n", unwrapped_content);
  return mrb_nil_value();
}


static mrb_value info(mrb_state *mrb, mrb_value self) {
	GXRModeObj *rmode = VIDEO_GetPreferredMode(NULL);
	printf("Screen Resolution: %dx%d\n", rmode->fbWidth, rmode->efbHeight);
  return mrb_nil_value();
}

static mrb_value reset_print_pos(mrb_state *mrb, mrb_value self) {
  printf("\x1b[0;0H");
  return mrb_nil_value();
}

//---------------------------------------------------------------------------------
void define_module_functions(mrb_state* mrb, struct RClass* mwii_module) {
printf("---xfb is: %lx---\n", (unsigned long)xfb);
  mrb_define_module_function(mrb, mwii_module, "print_msg", print_msg, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "get_button_masks", get_button_masks, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mwii_module, "draw20x20_640", draw20x20_640, MRB_ARGS_REQ(5));
  mrb_define_module_function(mrb, mwii_module, "init_controller_buffer", init_controller_buffer, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mwii_module, "start_controller_reader", start_controller_reader, MRB_ARGS_NONE());

  mrb_define_module_function(mrb, mwii_module, "get_button_state", get_button_state, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mwii_module, "get_button_states", get_button_states, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "start_btn?", start_btn, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "dpad_left?", dpad_left, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "dpad_right?", dpad_right, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "dpad_up?", dpad_up, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "dpad_down?", dpad_down, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "btn_a?", btn_a, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "btn_b?", btn_b, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "btn_start?", btn_start, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "get_current_button_index", get_current_button_index, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mwii_module, "waitvbl", waitvbl, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mwii_module, "get_next_button_state", get_next_button_state, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "content_string", content_string, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mwii_module, "render_png", render_png, MRB_ARGS_REQ(3));
  mrb_define_module_function(mrb, mwii_module, "draw_str", draw_str, MRB_ARGS_REQ(7));
  mrb_define_module_function(mrb, mwii_module, "info", info, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mwii_module, "reset_print_pos", reset_print_pos, MRB_ARGS_NONE());
}
