#include "util.h"


AppLogLevel global_log_level = APP_LOG_LEVEL_DEBUG;


AppLogLevel set_log_level(AppLogLevel level) {

    AppLogLevel old_log_level = global_log_level;
    global_log_level = level;

    return old_log_level;
}


AppLogLevel get_log_level() {

    return global_log_level;
}
