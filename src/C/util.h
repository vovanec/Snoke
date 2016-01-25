#include <pebble.h>


extern AppLogLevel global_log_level;


#define LOG(level, fmt, ...) {\
    if (level <= global_log_level) {\
        APP_LOG(level, fmt, ##__VA_ARGS__);\
    }\
}

AppLogLevel set_log_level(AppLogLevel level);
AppLogLevel get_log_level();
