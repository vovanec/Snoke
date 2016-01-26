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

#define MSG_SIZE 256


// Key values for AppMessage Dictionary
enum {
    STATUS_KEY = 0,
    MESSAGE_TYPE_KEY = 1,
    MESSAGE_KEY = 2
};

enum {
    MSG_TYPE_STOCKS = 0,
    MSG_TYPE_WEATHER = 1
};


enum {
    STATUS_OK = 0,
    STATUS_ERROR = 1
};


typedef void (*AppMessageHandler) (const char*);

static AppMessageHandler app_message_handlers[] = {

    set_stock_price,
    set_weather_info
};


void send_message(uint32_t message_type) {
    
    DictionaryIterator *iter;
    
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, STATUS_KEY, STATUS_OK);
    dict_write_uint8(iter, MESSAGE_TYPE_KEY, message_type);
    dict_write_end(iter);
    
    app_message_outbox_send();
}


static void on_message_received(DictionaryIterator *received, void *context) {
    
    Tuple *tuple;
    uint32_t message_type;
    uint8_t status;
    const char* message;
    
    tuple = dict_find(received, STATUS_KEY);
    if(!tuple) {
        LOG(APP_LOG_LEVEL_ERROR, "Could not extract status code from backend response.\n");
        return;
    }
    status = tuple->value->uint8;
    
    tuple = dict_find(received, MESSAGE_TYPE_KEY);
    if(!tuple) {
        LOG(APP_LOG_LEVEL_ERROR, "Could not extract message type from backend response.\n");
        return;
    }
    message_type = tuple->value->uint32;
    
    tuple = dict_find(received, MESSAGE_KEY);
    if(!tuple) {
        LOG(APP_LOG_LEVEL_ERROR, "Could not extract message from backend response.\n");
        return;
    }
    message = tuple->value->cstring;
    LOG(APP_LOG_LEVEL_DEBUG, "Received message type %ju, message string: %s.\n", (uintmax_t)message_type, message);
    
    if (status != STATUS_OK) {
        LOG(APP_LOG_LEVEL_ERROR, "Backend responded with status %d. Error message: %s.\n", status, message);
        return;
    }
    
    if (message_type < ARRAY_LENGTH(app_message_handlers)) {
        app_message_handlers[message_type](message);
    } else {
        LOG(APP_LOG_LEVEL_ERROR, "Received unknown message type %ju.\n", (uintmax_t)message_type);
    }
}


static void on_message_sent(DictionaryIterator *sent, void *context) {
    
    LOG(APP_LOG_LEVEL_DEBUG, "App message has been successfully sent.\n");
}


static void on_message_receive_failed(AppMessageResult reason, void *context) {
    
    LOG(APP_LOG_LEVEL_ERROR, "App message dropped, reason: %d\n", reason);
}


static void on_message_send_failed(DictionaryIterator *failed, AppMessageResult reason, void *context) {
    
    LOG(APP_LOG_LEVEL_ERROR, "Backend could not accept app message, reason: %d\n", reason);
}


static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
    
    if (tick_time->tm_min % 23 == 0) {
        LOG(APP_LOG_LEVEL_DEBUG, "Querying backend for stock info update.\n");
        send_message(MSG_TYPE_STOCKS);
    }
    
    if (tick_time->tm_min % 11 == 0) {
        LOG(APP_LOG_LEVEL_DEBUG, "Querying backend for weather info update.\n");
        send_message(MSG_TYPE_WEATHER);
    }
    
    set_time(tick_time);
    set_date(tick_time);
}


static void update_battery(BatteryChargeState charge_state) {
    
    set_battery_percent(charge_state.charge_percent);
}


static void bluetooth_connection_callback(bool connected) {
    
    set_bluetooth_connected(connected);
    vibes_long_pulse();
}


static void on_watch_shake(AccelAxisType axis, int32_t direction)
{
    LOG(APP_LOG_LEVEL_DEBUG, "Tap event. Axis type: %d, direction: %d\n", (int)axis, (int)direction);
    
    switch_screens();
}


void init(void) {
    
    create_ui();
    
    app_message_register_inbox_received(on_message_received);
    app_message_register_inbox_dropped(on_message_receive_failed);
    app_message_register_outbox_sent(on_message_sent);
    app_message_register_outbox_failed(on_message_send_failed);
    
    app_message_open(MSG_SIZE, MSG_SIZE);
    
    tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
    
    update_battery(battery_state_service_peek());
    battery_state_service_subscribe(&update_battery);
    
    set_bluetooth_connected(bluetooth_connection_service_peek());
    bluetooth_connection_service_subscribe(bluetooth_connection_callback);
    
    accel_tap_service_subscribe(on_watch_shake);
}


void deinit(void) {
    
    app_message_deregister_callbacks();
    tick_timer_service_unsubscribe();
    battery_state_service_unsubscribe();
    bluetooth_connection_service_unsubscribe();
    accel_tap_service_unsubscribe();
    
    destroy_ui();
}
