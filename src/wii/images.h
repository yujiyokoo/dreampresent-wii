#ifndef __images_h__
#define __images_h__

#include "DCSerialConnector_png.h"
#include "DCToSerial_png.h"
#include "bg_dark_png.h"
#include "dreamcast_photo_png.h"
#include "emulator_console_png.h"
#include "font_png.h"
#include "mruby_logo_32x32_png.h"
#include "soldered_connector_png.h"
#include "swirl_blue_32x28_png.h"
#include "yuji_avatar_png.h"
#include "ruby_white_blue_640x480_png.h"
#include "shisa1_png.h"
#include "shisa2_png.h"

const uint8_t *find_image_asset(char* image_name) {
  if(strcmp(image_name, "DCSerialConnector_png") == 0) {
    return DCSerialConnector_png;
  } else if(strcmp(image_name, "DCToSerial_png") == 0) {
    return DCToSerial_png;
  } else if(strcmp(image_name, "bg_dark_png") == 0) {
    return bg_dark_png;
  } else if(strcmp(image_name, "dreamcast_photo_png") == 0) {
    return dreamcast_photo_png;
  } else if(strcmp(image_name, "emulator_console_png") == 0) {
    return emulator_console_png;
  } else if(strcmp(image_name, "font_png") == 0) {
    return font_png;
  } else if(strcmp(image_name, "mruby_logo_32x32_png") == 0) {
    return mruby_logo_32x32_png;
  } else if(strcmp(image_name, "soldered_connector_png") == 0) {
    return soldered_connector_png;
  } else if(strcmp(image_name, "swirl_blue_32x28_png") == 0) {
    return swirl_blue_32x28_png;
  } else if(strcmp(image_name, "yuji_avatar_png") == 0) {
    return yuji_avatar_png;
  } else if(strcmp(image_name, "ruby_white_blue_640x480_png") == 0) {
    return ruby_white_blue_640x480_png;
  } else if(strcmp(image_name, "shisa1_png") == 0) {
    return shisa1_png;
  } else if(strcmp(image_name, "shisa2_png") == 0) {
    return shisa2_png;
  } else {
    // TODO: What should we return as default here?
    return swirl_blue_32x28_png;
  }
}
#endif
