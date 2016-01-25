#include <pebble.h>

#include "c/logic.h"
#include "c/util.h"


int main(void) {

    set_log_level(APP_LOG_LEVEL_WARNING);

    init();
    app_event_loop();
    deinit();
}