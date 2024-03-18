#include <grrlib.h>

#include <mruby.h>
#include <mruby/string.h>
#include <wiiuse/wpad.h>
#include "content_txt.h"

extern GRRLIB_texImg *tex_font;

// as puts is working now, this may not be needed any more
static mrb_value print_msg(mrb_state *mrb, mrb_value self) {
  char *unwrapped_content;
  mrb_value str_content;

  mrb_get_args(mrb, "S", &str_content);
  unwrapped_content = mrb_str_to_cstr(mrb, str_content);
  printf("%s\r\n", unwrapped_content);

  return mrb_nil_value();
}

static mrb_value content_string(mrb_state *mrb, mrb_value self) {
  return mrb_str_new_cstr(mrb, (const char*)content_txt);
}

static mrb_value render_screen_and_wait(mrb_state *mrb, mrb_value self) {
  GRRLIB_Render();
  VIDEO_WaitVSync();
  return mrb_nil_value();
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
  GRRLIB_Printf(x, y, tex_font, 0xFFFFFFFF, 1, unwrapped_content);
  return mrb_nil_value();
}

static mrb_value get_button_state(mrb_state *mrb, mrb_value self) {
  PAD_ScanPads();
  u16 btns = PAD_ButtonsHeld(0);
  return mrb_fixnum_value(btns);
}

static mrb_value btn_a(mrb_state *mrb, mrb_value self) {
  // unimplemented
  return mrb_bool_value(FALSE);
}

static mrb_value btn_b(mrb_state *mrb, mrb_value self) {
  // unimplemented
  return mrb_bool_value(FALSE);
}

static mrb_value btn_start(mrb_state *mrb, mrb_value self) {
  mrb_int state;
  mrb_get_args(mrb, "i", &state);

  return mrb_bool_value(state & PAD_BUTTON_START);
};

static mrb_value dpad_down(mrb_state *mrb, mrb_value self) {
  mrb_int state;
  mrb_get_args(mrb, "i", &state);

  return mrb_bool_value(state & PAD_BUTTON_DOWN);
}

static mrb_value dpad_right(mrb_state *mrb, mrb_value self) {
  mrb_int state;
  mrb_get_args(mrb, "i", &state);

  return mrb_bool_value(state & PAD_BUTTON_RIGHT);
}

static mrb_value dpad_left(mrb_state *mrb, mrb_value self) {
  mrb_int state;
  mrb_get_args(mrb, "i", &state);

  return mrb_bool_value(state & PAD_BUTTON_LEFT);
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

static mrb_value draw_horizontal_line(mrb_state *mrb, mrb_value self) {
  mrb_int x, y, len, r, g, b;
  mrb_get_args(mrb, "iiiiii", &x, &y, &len, &r, &g, &b);

  u32 rgb = PACK_PIXEL(r, g, b);

  GRRLIB_Line(x, y, x + len, y, rgb);

  return mrb_nil_value();
}

static mrb_value draw_vertical_line(mrb_state *mrb, mrb_value self) {
  mrb_int x, y, len, r, g, b;
  mrb_get_args(mrb, "iiiiii", &x, &y, &len, &r, &g, &b);

  u32 rgb = PACK_PIXEL(r, g, b);

  GRRLIB_Line(x, y, x, y + len, rgb);

  return mrb_nil_value();
}

void define_module_functions(mrb_state* mrb, struct RClass* mwii_module) {
  mrb_define_module_function(mrb, mwii_module, "print_msg", print_msg, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "content_string", content_string, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mwii_module, "render_png", render_png, MRB_ARGS_REQ(3));
  mrb_define_module_function(mrb, mwii_module, "draw_str", draw_str, MRB_ARGS_REQ(7));
  mrb_define_module_function(mrb, mwii_module, "get_button_state", get_button_state, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mwii_module, "btn_a?", btn_a, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "btn_b?", btn_b, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "btn_start?", btn_start, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "dpad_down?", dpad_down, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "dpad_right?", dpad_right, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "dpad_left?", dpad_left, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "render_screen_and_wait", render_screen_and_wait, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mwii_module, "draw_horizontal_line", draw_horizontal_line, MRB_ARGS_REQ(6));
  mrb_define_module_function(mrb, mwii_module, "draw_vertical_line", draw_vertical_line, MRB_ARGS_REQ(6));
}
