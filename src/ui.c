#include <pebble.h>
#include "ui.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
// END AUTO-GENERATED UI CODE

static GBitmap* s_snoke_bitmap;
//static GBitmap* s_battery_icon_bitmap;

#define TIME_STR_SIZE 16
static char TIME_STR[TIME_STR_SIZE];

#define BATTERY_PERCENT_SIZE 5
static char BATTERY_PERCENT_STR[BATTERY_PERCENT_SIZE];

#define STOCK_PRICE_SIZE 16
static char STOCK_PRICE_STR[STOCK_PRICE_SIZE];

#define WEATHER_SIZE 32
static char WEATHER_STR[WEATHER_SIZE];

static Window *s_window;
static BitmapLayer *s_bitmap_layer;
static TextLayer *s_stocks;
static TextLayer *s_time;
//static BitmapLayer *s_battery_icon;
static TextLayer *s_battery_percent;
static TextLayer *s_weather;
static GBitmap *s_bt_image;
static BitmapLayer *s_bt_layer;


static void handle_window_load(Window *window) {

    window_set_background_color(window, GColorBlack);

#ifndef PBL_SDK_3
    window_set_fullscreen(window, true);
#endif

    GFont s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
    GFont s_res_gothic_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
    GFont s_res_gothic_24 = fonts_get_system_font(FONT_KEY_GOTHIC_24);
    GFont s_res_gothic_28 = fonts_get_system_font(FONT_KEY_GOTHIC_28);

    Layer* root_layer = window_get_root_layer(window);

    // s_bitmaplayer
    s_bitmap_layer = bitmap_layer_create(GRect(0, 0, 113, 127));
    s_snoke_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SNOKE_BITMAP_SMALL);
    bitmap_layer_set_bitmap(s_bitmap_layer, s_snoke_bitmap);
    layer_add_child(root_layer, (Layer*)s_bitmap_layer);

    // s_stocks
    s_stocks = text_layer_create(GRect(59, 147, 80, 20));
    text_layer_set_background_color(s_stocks, GColorClear);
    text_layer_set_text_color(s_stocks, GColorWhite);
    text_layer_set_text_alignment(s_stocks, GTextAlignmentRight);
    text_layer_set_text(s_stocks, "");
    text_layer_set_font(s_stocks, s_res_gothic_18);
    layer_add_child(root_layer, (Layer *)s_stocks);

    // s_time
    s_time = text_layer_create(GRect(4, 138, 54, 32));
    text_layer_set_background_color(s_time, GColorClear);
    text_layer_set_text_color(s_time, GColorWhite);
    text_layer_set_text(s_time, "00:00");
    text_layer_set_font(s_time, s_res_gothic_28);
    layer_add_child(root_layer, (Layer *)s_time);

    // s_battery_icon
    //s_battery_icon = bitmap_layer_create(GRect(107, 5, 6, 12));
    //bitmap_layer_set_background_color(s_battery_icon, GColorWhite);
    //layer_add_child(root_layer, (Layer *)s_battery_icon);

    // s_battery_percent
    s_battery_percent = text_layer_create(GRect(115, 2, 28, 14));
    text_layer_set_background_color(s_battery_percent, GColorClear);
    text_layer_set_text_color(s_battery_percent, GColorWhite);
    text_layer_set_text(s_battery_percent, "100%");
    text_layer_set_text_alignment(s_battery_percent, GTextAlignmentCenter);
    text_layer_set_font(s_battery_percent, s_res_gothic_14);
    layer_add_child(root_layer, (Layer *)s_battery_percent);

    // s_weather
    s_weather = text_layer_create(GRect(7, 128, 132, 16));
    text_layer_set_background_color(s_weather, GColorClear);
    text_layer_set_text_color(s_weather, GColorWhite);
    text_layer_set_text(s_weather, "     ");
    text_layer_set_text_alignment(s_weather, GTextAlignmentCenter);
    text_layer_set_font(s_weather, s_res_gothic_14);
    layer_add_child(root_layer, (Layer *)s_weather);

    // Bluetooth icon.
    s_bt_image = gbitmap_create_with_resource(RESOURCE_ID_ICON_BLUETOOTH);
    s_bt_layer = bitmap_layer_create((GRect) {
        .origin = { .x = 2, .y = 2 },
        .size = gbitmap_get_bounds(s_bt_image).size});
    bitmap_layer_set_background_color(s_bt_layer, GColorClear);
    bitmap_layer_set_bitmap(s_bt_layer, s_bt_image);
    layer_add_child(root_layer, bitmap_layer_get_layer(s_bt_layer));

    APP_LOG(APP_LOG_LEVEL_DEBUG, "UI Initialized.\n");
}

static void handle_window_unload(Window* window) {

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroying UI.\n");

    window_destroy(window);

    bitmap_layer_destroy(s_bitmap_layer);
    gbitmap_destroy(s_snoke_bitmap);

    text_layer_destroy(s_stocks);

    text_layer_destroy(s_time);

    //bitmap_layer_destroy(s_battery_icon);
    text_layer_destroy(s_battery_percent);

    text_layer_destroy(s_weather);
    //gbitmap_destroy(s_battery_icon_bitmap);
}

void hide_ui(void) {
    window_stack_remove(s_window, true);
}

void show_ui(void) {

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Initializing UI.\n");

    s_window = window_create();

    window_set_window_handlers(s_window, (WindowHandlers) {
        .load = handle_window_load,
        .unload = handle_window_unload,
    });

    window_stack_push(s_window, true);
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


void set_bluetooth_connected(int connected) {

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting bluetooth indicator to %svisible.\n", connected ? "" : "in");
    layer_set_hidden(bitmap_layer_get_layer(s_bt_layer), !connected);
}
