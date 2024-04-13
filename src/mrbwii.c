#include <grrlib.h>

#include <ogc/lwp_watchdog.h>
#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/string.h>
#include <wiiuse/wpad.h>
#include "content_txt.h"
#include "images.h"

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

#define GRRLIB_WHITE   0xFFFFFFFF
static mrb_value render_png(mrb_state *mrb, mrb_value self) {
  mrb_value png_name;
  mrb_int base_x, base_y;
  char *c_png_name;

  mrb_get_args(mrb, "Sii", &png_name, &base_x, &base_y);
  c_png_name = mrb_str_to_cstr(mrb, png_name);
  printf("---- rendering png: %s \r", c_png_name);

  const uint8_t *png_asset = find_image_asset(c_png_name);

  // TODO: Seems like images should be only loaded once?
  GRRLIB_texImg *tex_png = GRRLIB_LoadTexture(png_asset);
  GRRLIB_DrawImg(base_x, base_y, tex_png, 0, 1, 1, GRRLIB_WHITE);
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

static mrb_value dpad_up(mrb_state *mrb, mrb_value self) {
  mrb_int state;
  mrb_get_args(mrb, "i", &state);

  return mrb_bool_value(state & PAD_BUTTON_UP);
}

static u32 PACK_PIXEL(u8 r, u8 g, u8 b, u8 a) {
  return (r << 24) | (g << 16) | (b << 8) | a;
}

static mrb_value draw_horizontal_line(mrb_state *mrb, mrb_value self) {
  mrb_int x, y, len, r, g, b;
  mrb_get_args(mrb, "iiiiii", &x, &y, &len, &r, &g, &b);

  u32 rgb = PACK_PIXEL(r, g, b, 255);

  GRRLIB_Line(x, y, x + len, y, rgb);

  return mrb_nil_value();
}

static mrb_value draw_vertical_line(mrb_state *mrb, mrb_value self) {
  mrb_int x, y, len, r, g, b;
  mrb_get_args(mrb, "iiiiii", &x, &y, &len, &r, &g, &b);

  u32 rgb = PACK_PIXEL(r, g, b, 255);

  GRRLIB_Line(x, y, x, y + len, rgb);

  return mrb_nil_value();
}

static mrb_value int_time(mrb_state *mrb, mrb_value self) {
  u64 curr_time = gettime();
  u32 curr_sec = ticks_to_millisecs(curr_time) / 1000;
  printf("curr_sec: %d\r", curr_sec);
  return mrb_int_value(mrb, curr_sec);
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
  mrb_define_module_function(mrb, mwii_module, "dpad_up?", dpad_up, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "render_screen_and_wait", render_screen_and_wait, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mwii_module, "draw_horizontal_line", draw_horizontal_line, MRB_ARGS_REQ(6));
  mrb_define_module_function(mrb, mwii_module, "draw_vertical_line", draw_vertical_line, MRB_ARGS_REQ(6));
  mrb_define_module_function(mrb, mwii_module, "int_time", int_time, MRB_ARGS_NONE());
}

// DreamPresentPng

static void mrb_dp_png_free(mrb_state *mrb, void *p) {
  GRRLIB_FreeTexture((GRRLIB_texImg *)p);
}

static const struct mrb_data_type mrb_dp_png_data_type = {
  "$i_mrb_dp_png_data_type", mrb_dp_png_free
};

static mrb_value render_dp_png(mrb_state *mrb, mrb_value self) {
  mrb_int x, y;
  mrb_get_args(mrb, "ii", &x, &y);
  GRRLIB_texImg *data = (GRRLIB_texImg*)DATA_PTR(self);

  GRRLIB_DrawImg(x, y, data, 0, 1, 1, GRRLIB_WHITE);

  return mrb_nil_value();
}

static mrb_value find_and_load_png(mrb_state *mrb, mrb_value self) {
  mrb_value png_name;
  char *c_png_name;

  mrb_get_args(mrb, "S", &png_name);
  c_png_name = mrb_str_to_cstr(mrb, png_name);

  const uint8_t *png_asset = find_image_asset(c_png_name);

  GRRLIB_texImg *tex_png = GRRLIB_LoadTexture(png_asset);
  mrb_data_init(self, tex_png, &mrb_dp_png_data_type);

  return self;
}

void dream_present_png_init(mrb_state *mrb) {
  struct RClass *dp_png_class = mrb_define_class(mrb, "DreamPresentPng", mrb->object_class);
  MRB_SET_INSTANCE_TT(dp_png_class, MRB_TT_DATA);

  mrb_define_method(mrb, dp_png_class, "initialize", find_and_load_png, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, dp_png_class, "render", render_dp_png, MRB_ARGS_REQ(2));
}

