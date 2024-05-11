#include <grrlib.h>

#include <stdlib.h> //abs
#include <ogc/lwp_watchdog.h>
#include <aesndlib.h>
#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/string.h>
#include <wiiuse/wpad.h>
#include "content_txt.h"
#include "images.h"
#include "sounds.h"

extern GRRLIB_texImg *tex_font;
extern AESNDPB *global_pb;

# define BUFSIZE 60
struct InputBuf {
  gforce_t buffer[BUFSIZE];
  uint8_t index;
} input_buf;

const int recent_frames_duration = 15;
int find_below_threshold(gforce_t* ghistory, int index, float threshold) {
  int start = (index + 60 - recent_frames_duration) % 60;
  for (int i = 0; i < recent_frames_duration; i++) {
    if (ghistory[(start + i) % 60].x < threshold) {
      return 1;
    }
  }
  return 0;
}

int find_above_threshold(gforce_t* ghistory, int index, float threshold) {
  int start = (index + 60 - recent_frames_duration) % 60;
  for (int i = 0; i < recent_frames_duration; i++) {
    if (ghistory[(start + i) % 60].x > threshold) {
      return 1;
    }
  }
  return 0;
}

int8_t movement_history[60] = {0};
void *record_input() {
  while(1) {
    VIDEO_WaitVSync();
    u64 curr_time = gettime();

    input_buf.index = (input_buf.index + 1) % BUFSIZE;
    WPAD_ScanPads();
    WPAD_GForce(0, &(input_buf.buffer[input_buf.index]));

    gforce_t gforce = input_buf.buffer[input_buf.index];
    movement_history[input_buf.index] = 0; // should use if-else-if-else but this is good enough
    // if it's near 0, and it's been positive recently, and been negative before that, then it's a neg_pos
    if(abs(gforce.x) < 0.5 && find_above_threshold(input_buf.buffer, input_buf.index, 1.0) && find_below_threshold(input_buf.buffer, (input_buf.index+45)%60, -2.0)) {
      movement_history[input_buf.index] = -1;
    }
    // if it's near 0, and it's been negative recently, and been positive before that, then it's a pos_neg
    if(abs(gforce.x) < 0.5 && find_below_threshold(input_buf.buffer, input_buf.index, -1.0) && find_above_threshold(input_buf.buffer, (input_buf.index+45)%60, 2.0)) {
      movement_history[input_buf.index] = 1;
    }

    LWP_YieldThread();
  }
}

void init_controller_reader() {
  input_buf.index = 0;
  for (int i = 0; i < BUFSIZE; i++) {
    input_buf.buffer[i] = (gforce_t){0.0, 0.0, 0.0};
  }
  lwp_t thread;
  LWP_CreateThread(&thread, record_input, NULL, NULL, 0, 0);
}

// 1 if horizontal movement -> stop detected
static mrb_value get_remote_state(mrb_state *mrb, mrb_value self) {
  static int8_t bookmark = -1;
  u64 curr_time = gettime();
  int8_t current_index = input_buf.index;
  static int8_t last_state = 0;

  // initialise to 0 if uninitialised
  if (bookmark == -1) {
    bookmark = 0;
  }
  // printf("bookmark: %d, current_index: %d\r", bookmark, current_index);
  // printf("last_state: %d, current_value: %d\r", last_state, movement_history[current_index]);

  // if it is not stopped yet, return 0
  if (movement_history[current_index] != 0) {
    // printf("returning 0\r");
    bookmark = current_index;
    last_state = 1;
    return mrb_fixnum_value(0);
  }

  // if the last state was 1, return 1 as it was moving and has stopped
  if (last_state == 1) {
    // printf("returning 1\r");
    bookmark = current_index;
    last_state = 0;
    return mrb_fixnum_value(1);
  }

  // if there has been movement between bookmark and current_index, return 1
  int8_t i;
  bool move_found = 0;
  for (i = bookmark; i != current_index; i = (i + 1) % BUFSIZE) {
    if (movement_history[i] == 1 || movement_history[i] == -1) {
      // movement detected
      move_found = 1;
    }
  }

  // printf("returning mov_found: %d\r", move_found);
  last_state = 0;
  bookmark = current_index;
  return mrb_fixnum_value(move_found);
}

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

static u32 build_rgba(u8 r, u8 g, u8 b, u8 a) {
  return ((u32)r << 24) | (((u32)g << 16) ) | (((u32)b << 8) ) | ((u32)a );
}

static mrb_value draw_str(mrb_state *mrb, mrb_value self) {
  char *unwrapped_content;
  mrb_value str_content;
  mrb_int x, y, r, g, b, bg_on;

  mrb_get_args(mrb, "Siiiiii", &str_content, &x, &y, &r, &g, &b, &bg_on);
  unwrapped_content = mrb_str_to_cstr(mrb, str_content);

  u32 rgba = build_rgba((u8)r, (u8)g, (u8)b, 255);

  // render black background of the text width * the height of 16 x 1.5
  if (bg_on) {
    GRRLIB_Rectangle(x, y, (int)(strlen(unwrapped_content) * (8*1.5)), (int)((16*1.5)), build_rgba(0, 0, 0, 255), true);
  }

  GRRLIB_Printf(x, y, tex_font, rgba, 1.5, unwrapped_content);
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

static mrb_value draw_horizontal_line(mrb_state *mrb, mrb_value self) {
  mrb_int x, y, len, r, g, b;
  mrb_get_args(mrb, "iiiiii", &x, &y, &len, &r, &g, &b);

  u32 rgb = build_rgba(r, g, b, 255);

  GRRLIB_Line(x, y, x + len, y, rgb);

  return mrb_nil_value();
}

static mrb_value draw_vertical_line(mrb_state *mrb, mrb_value self) {
  mrb_int x, y, len, r, g, b;
  mrb_get_args(mrb, "iiiiii", &x, &y, &len, &r, &g, &b);

  u32 rgb = build_rgba(r, g, b, 255);

  GRRLIB_Line(x, y, x, y + len, rgb);

  return mrb_nil_value();
}

static mrb_value int_time(mrb_state *mrb, mrb_value self) {
  u64 curr_time = gettime();
  u32 curr_sec = ticks_to_millisecs(curr_time) / 1000;
  // printf("curr_sec: %d\r", curr_sec);
  return mrb_int_value(mrb, curr_sec);
}

static mrb_value play_test_sound(mrb_state *mrb, mrb_value self) {
  // play test sound
  if(global_pb == NULL) {
    global_pb = AESND_AllocateVoice(NULL);
  }
  AESND_PlayVoice(global_pb, VOICE_STEREO16, test_wav_raw, test_wav_raw_size, 44100, 0, 0);
  return mrb_nil_value();
}

void define_module_functions(mrb_state* mrb, struct RClass* mwii_module) {
  mrb_define_module_function(mrb, mwii_module, "print_msg", print_msg, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, mwii_module, "content_string", content_string, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mwii_module, "render_png", render_png, MRB_ARGS_REQ(3));
  mrb_define_module_function(mrb, mwii_module, "draw_str", draw_str, MRB_ARGS_REQ(7));
  mrb_define_module_function(mrb, mwii_module, "get_button_state", get_button_state, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, mwii_module, "get_remote_state", get_remote_state, MRB_ARGS_NONE());
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
  mrb_define_module_function(mrb, mwii_module, "play_test_sound", play_test_sound, MRB_ARGS_NONE());
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

