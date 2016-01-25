#include <pebble.h>

#include "C/logic.h"
#include "C/util.h"


int main(void) {

    set_log_level(APP_LOG_LEVEL_DEBUG);

    init();
    app_event_loop();
    deinit();
}