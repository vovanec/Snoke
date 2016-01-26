/*
 
 The MIT License (MIT)
 
 Copyright © 2016 Volodymyr Kuznetsov
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */


#include <pebble.h>
#include "ui.h"
#include "util.h"
#include "scroll-text-layer.h"
#include "quotes.h"


#define TIME_STR_SIZE 16
static char TIME_STR[TIME_STR_SIZE];

#define DATE_STR_SIZE 16
static char DATE_STR[DATE_STR_SIZE];

#define BATTERY_PERCENT_SIZE 5
static char BATTERY_PERCENT_STR[BATTERY_PERCENT_SIZE];

#define STOCK_PRICE_SIZE 16
static char STOCK_PRICE_STR[STOCK_PRICE_SIZE];

#define WEATHER_SIZE 32
static char WEATHER_STR[WEATHER_SIZE];

#define ONE_MINUTE 60 * 1000
#define PERSIST_CURRENT_SITH_QUOTE_KEY 0


typedef void(*WatchfaceWindowFunc)(void);


static Window *s_window;

static BitmapLayer *s_snoke_bitmap_layer;
static BitmapLayer *s_alt_snoke_bitmap_layer;
static BitmapLayer *s_battery_icon_layer;
static BitmapLayer *s_bt_layer;

static GBitmap *s_snoke_bitmap;
static GBitmap *s_alt_snoke_bitmap;
static GBitmap *s_bt_bitmap;

static TextLayer *s_stocks_layer;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_battery_percent_layer;
static TextLayer *s_weather_layer;

static ScrollTextLayer *s_scroll_text_layer;
static AppTimer *s_scroll_timer;

static uint16_t s_current_quote = 0;
static uint8_t s_current_window = 0;


static void handle_window_load(Window *window) {
    
    window_set_background_color(window, GColorBlack);
    
    GFont s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
    GFont s_res_gothic_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
    GFont s_res_gothic_24 = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
    GFont s_res_gothic_28 = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
    
    Layer* root_layer = window_get_root_layer(window);
    
    // s_bitmaplayer
    s_snoke_bitmap_layer = bitmap_layer_create(GRect(0, 26, 144, 100));
    s_snoke_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SNOKE_BITMAP_WIDE);
    bitmap_layer_set_bitmap(s_snoke_bitmap_layer, s_snoke_bitmap);
    layer_add_child(root_layer, (Layer*)s_snoke_bitmap_layer);
    
    s_alt_snoke_bitmap_layer = bitmap_layer_create(GRect(0, 0, 113, 127));
    s_alt_snoke_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SNOKE_BITMAP_SMALL);
    bitmap_layer_set_bitmap(s_alt_snoke_bitmap_layer, s_alt_snoke_bitmap);
    layer_set_hidden((Layer*)s_alt_snoke_bitmap_layer, true);
    layer_add_child(root_layer, (Layer*)s_alt_snoke_bitmap_layer);
    
    // s_stocks_layer
    s_stocks_layer = text_layer_create(GRect(59, 147, 80, 20));
    text_layer_set_background_color(s_stocks_layer, GColorClear);
    text_layer_set_text_color(s_stocks_layer, GColorWhite);
    text_layer_set_text_alignment(s_stocks_layer, GTextAlignmentRight);
    text_layer_set_text(s_stocks_layer, "");
    text_layer_set_font(s_stocks_layer, s_res_gothic_18);
    layer_add_child(root_layer, (Layer *)s_stocks_layer);
    
    // s_time_layer
    s_time_layer = text_layer_create(GRect(4, 138, 54, 32));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorWhite);
    text_layer_set_text(s_time_layer, "00:00");
    text_layer_set_font(s_time_layer, s_res_gothic_28);
    layer_add_child(root_layer, (Layer *)s_time_layer);
    
    // s_date_layer
    s_date_layer = text_layer_create(GRect(20, 2, 80, 16));
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_color(s_date_layer, GColorWhite);
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
    text_layer_set_text(s_date_layer, "");
    text_layer_set_font(s_date_layer, s_res_gothic_14);
    layer_add_child(root_layer, (Layer *)s_date_layer);
    
    // s_battery_icon_layer
    s_battery_icon_layer = bitmap_layer_create(GRect(110, 6, 4, 11));
    bitmap_layer_set_background_color(s_battery_icon_layer, GColorWhite);
    layer_add_child(root_layer, (Layer *)s_battery_icon_layer);
    
    // s_battery_percent_layer
    s_battery_percent_layer = text_layer_create(GRect(115, 2, 32, 14));
    text_layer_set_background_color(s_battery_percent_layer, GColorClear);
    text_layer_set_text_color(s_battery_percent_layer, GColorWhite);
    text_layer_set_text_alignment(s_battery_percent_layer, GTextAlignmentLeft);
    text_layer_set_font(s_battery_percent_layer, s_res_gothic_14);
    layer_add_child(root_layer, (Layer *)s_battery_percent_layer);
    
    // s_weather_layer
    s_weather_layer = text_layer_create(GRect(7, 128, 132, 16));
    text_layer_set_background_color(s_weather_layer, GColorClear);
    text_layer_set_text_color(s_weather_layer, GColorWhite);
    text_layer_set_text(s_weather_layer, "     ");
    text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
    text_layer_set_font(s_weather_layer, s_res_gothic_14);
    layer_add_child(root_layer, (Layer *)s_weather_layer);
    
    // Bluetooth icon.
    s_bt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_BLUETOOTH);
    s_bt_layer = bitmap_layer_create((GRect) {
        .origin = { .x = 2, .y = 2 },
        .size = gbitmap_get_bounds(s_bt_bitmap).size});
    bitmap_layer_set_background_color(s_bt_layer, GColorClear);
    bitmap_layer_set_compositing_mode(s_bt_layer, GCompOpSet);
    bitmap_layer_set_bitmap(s_bt_layer, s_bt_bitmap);
    layer_add_child(root_layer, bitmap_layer_get_layer(s_bt_layer));
    
    s_scroll_text_layer = scroll_text_layer_create(GRect(0, 26, 144, 100));
    scroll_text_layer_set_text(s_scroll_text_layer, SITH_QUOTES[0]);
    scroll_text_layer_set_background_color(s_scroll_text_layer, GColorBlack);
    scroll_text_layer_set_text_color(s_scroll_text_layer, GColorWhite);
    scroll_text_layer_set_system_font(s_scroll_text_layer, FONT_KEY_GOTHIC_18);
    scroll_text_layer_set_text_alignment(s_scroll_text_layer, GTextAlignmentCenter);
    scroll_text_layer_set_hidden(s_scroll_text_layer, true);
    scroll_text_layer_add_to_window(s_scroll_text_layer, window);
    scroll_text_layer_autoscroll_start(s_scroll_text_layer, 5000, 200, 2);
    
    LOG(APP_LOG_LEVEL_DEBUG, "UI Initialized.\n");
}


static void handle_window_unload(Window* window) {
    
    LOG(APP_LOG_LEVEL_DEBUG, "Destroying UI.\n");
    
    window_destroy(window);
    
    gbitmap_destroy(s_snoke_bitmap);
    bitmap_layer_destroy(s_snoke_bitmap_layer);

    gbitmap_destroy(s_alt_snoke_bitmap);
    bitmap_layer_destroy(s_alt_snoke_bitmap_layer);

    text_layer_destroy(s_stocks_layer);
    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_date_layer);
    text_layer_destroy(s_weather_layer);
    text_layer_destroy(s_battery_percent_layer);
    
    bitmap_layer_destroy(s_battery_icon_layer);

    gbitmap_destroy(s_bt_bitmap);
    bitmap_layer_destroy(s_bt_layer);

    scroll_text_layer_destroy(s_scroll_text_layer);
}


void create_ui(void) {
    
    LOG(APP_LOG_LEVEL_DEBUG, "Initializing UI.\n");
    
    s_current_quote = persist_exists(PERSIST_CURRENT_SITH_QUOTE_KEY) ?
        persist_read_int(PERSIST_CURRENT_SITH_QUOTE_KEY) : 0;
    
    s_window = window_create();
    
    window_set_window_handlers(s_window, (WindowHandlers) {
        .load = handle_window_load,
        .unload = handle_window_unload,
    });
    
    window_stack_push(s_window, true);
}


void destroy_ui(void) {
    
    window_stack_remove(s_window, true);
    persist_write_int(PERSIST_CURRENT_SITH_QUOTE_KEY, s_current_quote);
}


void set_stock_price(const char* price_string) {
    
    strncpy(STOCK_PRICE_STR, price_string, STOCK_PRICE_SIZE);
    STOCK_PRICE_STR[STOCK_PRICE_SIZE - 1] = '\0';
    LOG(APP_LOG_LEVEL_DEBUG, "Setting current stock price to %s\n", STOCK_PRICE_STR);
    text_layer_set_text(s_stocks_layer, STOCK_PRICE_STR);
}


void set_weather_info(const char* weather) {
    
    strncpy(WEATHER_STR, weather, WEATHER_SIZE);
    WEATHER_STR[WEATHER_SIZE - 1] = '\0';
    LOG(APP_LOG_LEVEL_DEBUG, "Setting current weather info to %s\n", WEATHER_STR);
    text_layer_set_text(s_weather_layer, WEATHER_STR);
}


void set_time(const struct tm *tick_time) {
    
    strftime(TIME_STR, TIME_STR_SIZE, "%H:%M", tick_time);
    LOG(APP_LOG_LEVEL_DEBUG, "Setting current time to %s\n", TIME_STR);
    text_layer_set_text(s_time_layer, TIME_STR);
}


void set_date(const struct tm *tick_time) {
    
    strftime(DATE_STR, DATE_STR_SIZE, "%a, %b %d", tick_time);
    LOG(APP_LOG_LEVEL_DEBUG, "Setting current date to %s\n", DATE_STR);
    text_layer_set_text(s_date_layer, DATE_STR);
}


void set_battery_percent(int battery_percent) {
    
    snprintf(BATTERY_PERCENT_STR, BATTERY_PERCENT_SIZE, "%d%%", battery_percent);
    LOG(APP_LOG_LEVEL_DEBUG, "Setting battery percent to %s\n", BATTERY_PERCENT_STR);
    text_layer_set_text(s_battery_percent_layer, BATTERY_PERCENT_STR);
}


void set_bluetooth_connected(int connected) {
    
    LOG(APP_LOG_LEVEL_DEBUG, "Setting bluetooth indicator to %svisible.\n", connected ? "" : "in");
    layer_set_hidden(bitmap_layer_get_layer(s_bt_layer), !connected);
}


static void on_scroll_text_timeout(void *data) {
    
    LOG(APP_LOG_LEVEL_DEBUG, "Timer, switching back to the first screen.\n");
    
    s_scroll_timer = NULL;
    s_current_window = -1;
    
    switch_screens();
}


static void show_snoke_bitmap_primary() {

    scroll_text_layer_set_hidden(s_scroll_text_layer, true);
    layer_set_hidden((Layer*)s_alt_snoke_bitmap_layer, true);
    
    layer_set_hidden((Layer*)s_snoke_bitmap_layer, false);
    layer_set_hidden((Layer*)s_date_layer, false);
}


static void show_snoke_bitmap_secondary() {

    layer_set_hidden((Layer*)s_snoke_bitmap_layer, true);
    layer_set_hidden((Layer*)s_date_layer, true);
    scroll_text_layer_set_hidden(s_scroll_text_layer, true);
    
    layer_set_hidden((Layer*)s_alt_snoke_bitmap_layer, false);
    
    s_scroll_timer = app_timer_register(ONE_MINUTE, on_scroll_text_timeout, NULL);
}


static void show_sith_quotes() {

    layer_set_hidden((Layer*)s_snoke_bitmap_layer, true);
    layer_set_hidden((Layer*)s_alt_snoke_bitmap_layer, true);
    
    layer_set_hidden((Layer*)s_date_layer, false);
    
    scroll_text_layer_set_text(s_scroll_text_layer, SITH_QUOTES[s_current_quote++]);
    if(s_current_quote >= NUM_OF_QUOTES) {
        s_current_quote = 0;
    }
    scroll_text_layer_set_hidden(s_scroll_text_layer, false);
    
    s_scroll_timer = app_timer_register(ONE_MINUTE, on_scroll_text_timeout, NULL);
}


static const WatchfaceWindowFunc watchface_window_funcs[] = {

    show_snoke_bitmap_primary,
#if defined(PBL_BW)
    show_snoke_bitmap_secondary,
#endif
    show_sith_quotes
};


void switch_screens() {
    
    if(s_scroll_timer) {
        app_timer_cancel(s_scroll_timer);
        s_scroll_timer = NULL;
    }
    
    if(++s_current_window >= ARRAY_LENGTH(watchface_window_funcs)) {
        s_current_window = 0;
    }
    
    LOG(APP_LOG_LEVEL_DEBUG, "Switching current window to %d\n", s_current_window);
    watchface_window_funcs[s_current_window]();
}
