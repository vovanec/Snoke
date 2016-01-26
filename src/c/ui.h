#include <time.h>

void create_ui(void);
void destroy_ui(void);
void set_stock_price(const char* price_string);
void set_weather_info(const char* weather);
void set_time(const struct tm *tick_time);
void set_date(const struct tm *tick_time);
void set_battery_percent(int battery_percent);
void set_bluetooth_connected(int connected);
void switch_screens(void);
