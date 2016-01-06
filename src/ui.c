#include <pebble.h>
#include "ui.h"

static GBitmap* s_snoke_bitmap;

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_14;
static BitmapLayer *s_bitmaplayer;
static TextLayer *s_stocks_label;
static TextLayer *s_stocks;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  
  s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  // s_bitmaplayer
  s_bitmaplayer = bitmap_layer_create(GRect(0, 0, 144, 168));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer);
  
  // s_stocks_label
  s_stocks_label = text_layer_create(GRect(64, 152, 32, 16));
  text_layer_set_background_color(s_stocks_label, GColorClear);
  text_layer_set_text_color(s_stocks_label, GColorWhite);
  text_layer_set_text(s_stocks_label, "JNPR: ");
  text_layer_set_font(s_stocks_label, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_stocks_label);
  
  // s_stocks
  s_stocks = text_layer_create(GRect(96, 152, 48, 16));
  text_layer_set_background_color(s_stocks, GColorClear);
  text_layer_set_text_color(s_stocks, GColorWhite);
  text_layer_set_text(s_stocks, "loading...");
  text_layer_set_text_alignment(s_stocks, GTextAlignmentCenter);
  text_layer_set_font(s_stocks, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_stocks);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  bitmap_layer_destroy(s_bitmaplayer);
  text_layer_destroy(s_stocks_label);
  text_layer_destroy(s_stocks);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroying UI\n");
  
  gbitmap_destroy(s_snoke_bitmap);
  destroy_ui();
}

void show_ui(void) {
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Initializing UI\n");
  initialise_ui();
  
  // HACK: UI editor does not allow to set bitmap...
  s_snoke_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SNOKE_BITMAP);
  bitmap_layer_set_bitmap(s_bitmaplayer, s_snoke_bitmap);
  
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_ui(void) {
  window_stack_remove(s_window, true);
}

Window* get_ui(void) {
  return s_window;
}

