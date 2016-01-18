#include <pebble.h>

#include "ui.h"


// Key values for AppMessage Dictionary
enum {
    STATUS_KEY = 0,
    MESSAGE_TYPE_KEY = 1,
    MESSAGE_KEY = 2
};

enum {
    MSG_TYPE_STOCKS = 0,
    MSG_TYPE_WEATHER = 1,
};

enum {
    STATUS_OK = 0,
    STATUS_ERROR = 1
};

// Write message to buffer & send
void query_backend(int message_type) {

    DictionaryIterator *iter;

    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, STATUS_KEY, STATUS_OK);
    dict_write_uint8(iter, MESSAGE_TYPE_KEY, message_type);
    dict_write_end(iter);

    app_message_outbox_send();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Message has been sent\n");
}

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {

    Tuple *tuple;
    int status;
    int message_type;
    char* message;

    tuple = dict_find(received, STATUS_KEY);
    if(!tuple) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not extract status code from backend response.\n");
        return;
    }
    status = (int)tuple->value->uint32;

    tuple = dict_find(received, MESSAGE_TYPE_KEY);
    if(!tuple) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not extract message type from backend response.\n");
        return;
    }
    message_type = (int)tuple->value->uint32;

    tuple = dict_find(received, MESSAGE_KEY);
    if(!tuple) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Could not extract message from backend response.\n");
        return;
    }
    message = tuple->value->cstring;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Received message type %d, message string: %s.\n", message_type, message);

    if (status != STATUS_OK) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Backend responded with status %d. Error message: %s.\n", status, message);
        return;
    }

    switch(message_type) {
    case MSG_TYPE_STOCKS:
        set_stock_price(message);
        break;
    case MSG_TYPE_WEATHER:
        set_weather_info(message);
        break;
    default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Received unknown message type %d.\n", message_type);
    }
}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {

    APP_LOG(APP_LOG_LEVEL_ERROR, "App message dropped.\n");
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {

    APP_LOG(APP_LOG_LEVEL_ERROR, "Backend could not accept app message.\n");
}


static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {

    //APP_LOG(APP_LOG_LEVEL_DEBUG, "Tick received: %d:%d\n", tick_time->tm_hour, tick_time->tm_min);

    if (tick_time->tm_min % 11 == 0) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Querying backend for stock info update.\n");
        query_backend(MSG_TYPE_STOCKS);
    }

    if (tick_time->tm_min % 6 == 0) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Querying backend for weather info update.\n");
        query_backend(MSG_TYPE_WEATHER);
    }

    set_time(tick_time);
}

static void update_battery(BatteryChargeState charge_state) {

    set_battery_percent(charge_state.charge_percent);
}

static void bluetooth_connection_callback(bool connected) {

    set_bluetooth_connected(connected);

    if(!connected) {
         vibes_long_pulse();
    }
}


static void tap_handler(AccelAxisType axis, int32_t direction)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "TAP HANDLER");
}


void init(void) {

    show_ui();

    app_message_register_inbox_received(in_received_handler);
    app_message_register_inbox_dropped(in_dropped_handler);
    app_message_register_outbox_failed(out_failed_handler);

    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

    tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);

    update_battery(battery_state_service_peek());
    battery_state_service_subscribe(&update_battery);

    set_bluetooth_connected(bluetooth_connection_service_peek());
    bluetooth_connection_service_subscribe(bluetooth_connection_callback);

    // Sample as little as often to save battery and no need for precision
    accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
    accel_tap_service_subscribe(tap_handler);
}

void deinit(void) {

    app_message_deregister_callbacks();
    tick_timer_service_unsubscribe();
    battery_state_service_unsubscribe();
    bluetooth_connection_service_unsubscribe();

    accel_tap_service_unsubscribe();

    hide_ui();
}

int main( void ) {
    init();
    app_event_loop();
    deinit();
}