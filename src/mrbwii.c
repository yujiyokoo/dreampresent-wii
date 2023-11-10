#include <mruby.h>
#include <mruby/string.h>

static mrb_value print_msg(mrb_state *mrb, mrb_value self) {
  char *unwrapped_content;
  mrb_value str_content;

  mrb_get_args(mrb, "S", &str_content);
  unwrapped_content = mrb_str_to_cstr(mrb, str_content);
  printf("\x1b[10;2H");
  printf("%s\n", unwrapped_content);

  return mrb_nil_value();
}

static mrb_value content_string(mrb_state *mrb, mrb_value self) {
  return mrb_str_new_cstr(mrb, "--");
}

static mrb_value render_png(mrb_state *mrb, mrb_value self) {
  // unimplemented
  return mrb_nil_value();
}

static mrb_value draw_str(mrb_state *mrb, mrb_value self) {
  // unimplemented
  return mrb_nil_value();
}

static mrb_value get_button_state(mrb_state *mrb, mrb_value self) {
  // unimplemented
  return mrb_nil_value();
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
  // unimplemented
  return mrb_bool_value(FALSE);
};

void define_module_functions(mrb_state* mrb, struct RClass* module) {
  mrb_define_module_function(mrb, module, "print_msg", print_msg, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, module, "content_string", content_string, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, module, "render_png", render_png, MRB_ARGS_REQ(3));
  mrb_define_module_function(mrb, module, "draw_str", draw_str, MRB_ARGS_REQ(7));
  mrb_define_module_function(mrb, module, "get_button_state", get_button_state, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, module, "btn_a?", btn_a, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, module, "btn_b?", btn_b, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, module, "btn_start?", btn_start, MRB_ARGS_REQ(1));
}

