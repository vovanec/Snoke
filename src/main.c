#include <pebble.h>

#include "c/logic.h"
#include "c/util.h"


int main(void) {

    set_log_level(APP_LOG_LEVEL_DEBUG);

    init();
    app_event_loop();
    deinit();
}