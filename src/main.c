#include <pebble.h>

#include "ui.h"

	
// Key values for AppMessage Dictionary
enum {
	STATUS_KEY = 0,	
	MESSAGE_KEY = 1
};

enum {
  GET_STOCKS = 0,
  GET_WEATHER = 1
};

enum {
  STATUS_OK = 0,
  STATUS_ERROR = 1
};

// Write message to buffer & send
void send_message(void){
	DictionaryIterator *iter;
	
	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, STATUS_KEY, STATUS_OK);
	dict_write_uint8(iter, MESSAGE_KEY, GET_STOCKS);  
	dict_write_end(iter);
  
  app_message_outbox_send();
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Message has been sent\n");
}

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *tuple;
	
	tuple = dict_find(received, STATUS_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Status: %d", (int)tuple->value->uint32); 
	}
	
	tuple = dict_find(received, MESSAGE_KEY);
	if(tuple) {
    char* stock_price = tuple->value->cstring;
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Message: %s", stock_price);
    set_stock_price(stock_price);
	}}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {	
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}


static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Tick received: %d:%d\n", tick_time->tm_hour, tick_time->tm_min);
  set_time(tick_time);
  send_message();
}

void init(void) {
  
  show_ui();    
	
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);
		
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  tick_timer_service_subscribe(HOUR_UNIT, handle_tick);
	
}

void deinit(void) {
    
	app_message_deregister_callbacks();
  hide_ui();
}

int main( void ) {
	init();
	app_event_loop();
	deinit();
}