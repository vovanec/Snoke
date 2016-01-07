#include <pebble.h>
#include "ui.h"

static GBitmap* s_snoke_bitmap;
static GBitmap* s_battery_icon_bitmap;

#define TIME_STR_SIZE 16
static char TIME_STR[TIME_STR_SIZE];

#define BATTERY_PERCENT_SIZE 5
static char BATTERY_PERCENT_STR[BATTERY_PERCENT_SIZE];

#define STOCK_PRICE_SIZE 16
static char STOCK_PRICE_STR[STOCK_PRICE_SIZE];

#define WEATHER_SIZE 32
static char WEATHER_STR[WEATHER_SIZE];


// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_14;
static GFont s_res_gothic_24;
static BitmapLayer *s_bitmaplayer;
static TextLayer *s_stocks_label;
static TextLayer *s_stocks;
static TextLayer *s_time;
static BitmapLayer *s_battery_icon;
static TextLayer *s_battery_percent;
static TextLayer *s_weather;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  
  s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  s_res_gothic_24 = fonts_get_system_font(FONT_KEY_GOTHIC_24);
  // s_bitmaplayer
  s_bitmaplayer = bitmap_layer_create(GRect(0, 0, 144, 168));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer);
  
  // s_stocks_label
  s_stocks_label = text_layer_create(GRect(78, 152, 32, 16));
  text_layer_set_background_color(s_stocks_label, GColorClear);
  text_layer_set_text_color(s_stocks_label, GColorWhite);
  text_layer_set_text(s_stocks_label, "JNPR: ");
  text_layer_set_font(s_stocks_label, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_stocks_label);
  
  // s_stocks
  s_stocks = text_layer_create(GRect(111, 152, 32, 16));
  text_layer_set_background_color(s_stocks, GColorClear);
  text_layer_set_text_color(s_stocks, GColorWhite);
  text_layer_set_text(s_stocks, "    ");
  text_layer_set_text_alignment(s_stocks, GTextAlignmentRight);
  text_layer_set_font(s_stocks, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_stocks);
  
  // s_time
  s_time = text_layer_create(GRect(4, 142, 49, 24));
  text_layer_set_background_color(s_time, GColorClear);
  text_layer_set_text_color(s_time, GColorWhite);
  text_layer_set_text(s_time, "00:00");
  text_layer_set_font(s_time, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_time);
  
  // s_battery_icon
  s_battery_icon = bitmap_layer_create(GRect(107, 5, 6, 12));
  bitmap_layer_set_background_color(s_battery_icon, GColorWhite);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_battery_icon);
  
  // s_battery_percent
  s_battery_percent = text_layer_create(GRect(115, 3, 28, 14));
  text_layer_set_background_color(s_battery_percent, GColorClear);
  text_layer_set_text_color(s_battery_percent, GColorWhite);
  text_layer_set_text(s_battery_percent, "100%");
  text_layer_set_text_alignment(s_battery_percent, GTextAlignmentCenter);
  text_layer_set_font(s_battery_percent, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_battery_percent);
  
  // s_weather
  s_weather = text_layer_create(GRect(7, 128, 132, 16));
  text_layer_set_background_color(s_weather, GColorClear);
  text_layer_set_text_color(s_weather, GColorWhite);
  text_layer_set_text(s_weather, "     ");
  text_layer_set_text_alignment(s_weather, GTextAlignmentCenter);
  text_layer_set_font(s_weather, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_weather);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  bitmap_layer_destroy(s_bitmaplayer);
  text_layer_destroy(s_stocks_label);
  text_layer_destroy(s_stocks);
  text_layer_destroy(s_time);
  bitmap_layer_destroy(s_battery_icon);
  text_layer_destroy(s_battery_percent);
  text_layer_destroy(s_weather);
}
// END AUTO-GENERATED UI CODE

void init_custom_resources(void) {
  // HACK: UI editor does not allow to set bitmap...
  s_snoke_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SNOKE_BITMAP);
  bitmap_layer_set_bitmap(s_bitmaplayer, s_snoke_bitmap);
  s_battery_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_BATTERY_VERT);
  bitmap_layer_set_bitmap(s_battery_icon, s_battery_icon_bitmap);  
}

void destroy_custom_resources(void) {
  gbitmap_destroy(s_snoke_bitmap);
  gbitmap_destroy(s_battery_icon_bitmap);
}

static void handle_window_unload(Window* window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroying UI\n");
  
  destroy_custom_resources();
  destroy_ui();
}

void show_ui(void) {
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Initializing UI\n");
  initialise_ui();
  init_custom_resources();
  
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "UI Initialized\n");  
}

void hide_ui(void) {
  window_stack_remove(s_window, true);
}

Window* get_ui(void) {
  return s_window;
}

void set_stock_price(char* price_string) {
  
  strncpy(STOCK_PRICE_STR, price_string, STOCK_PRICE_SIZE);
  STOCK_PRICE_STR[STOCK_PRICE_SIZE - 1] = '\0';
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting current stock price to %s\n", STOCK_PRICE_STR);  
  text_layer_set_text(s_stocks, STOCK_PRICE_STR);
}

void set_weather_info(char* weather) {
  
  strncpy(WEATHER_STR, weather, WEATHER_SIZE);
  WEATHER_STR[WEATHER_SIZE - 1] = '\0';
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting current weather info to %s\n", WEATHER_STR);    
  text_layer_set_text(s_weather, WEATHER_STR);
}

void set_time(struct tm *tick_time) {
  
  strftime(TIME_STR, TIME_STR_SIZE, "%H:%M", tick_time);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting current time to %s\n", TIME_STR);
  text_layer_set_text(s_time, TIME_STR);
}

void set_battery_percent(int battery_percent) {
  snprintf(BATTERY_PERCENT_STR, BATTERY_PERCENT_SIZE, "%d%%", battery_percent);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting battery percent to %s\n", BATTERY_PERCENT_STR);
  text_layer_set_text(s_battery_percent, BATTERY_PERCENT_STR);
}