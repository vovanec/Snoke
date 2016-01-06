#include <pebble.h>
#include <time.h>

void show_ui(void);
void hide_ui(void);
Window* get_ui(void);
void set_stock_price(char* price_string);
void set_time(struct tm *tick_time);
