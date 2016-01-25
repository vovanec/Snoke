/*
 
 Scroll Text Layer v1.0
 A Pebble library for having a scrollable text layer in your Pebble apps.
 http://smallstoneapps.github.io/scroll-text-layer/
 
 ----------------------
 
 The MIT License (MIT)
 
 Copyright © 2013-2014 Matthew Tole
 
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
 
 --------------------
 
 src/scroll-text-layer.c
 
 */

#include <stdint.h>
#include <pebble.h>
#include "scroll-text-layer.h"

#define MAX_HEIGHT 2000
#define PADDING_X 2
#define PADDING_Y 2


struct ScrollTextLayer {
    ScrollLayer* scroll_layer;
    TextLayer* text_layer;
    AppTimer *scroll_timer;
    uint16_t scroll_delay;
    uint16_t scroll_interval;
    uint16_t scroll_offset;
    bool hidden;
};


static void scroll_text_layer_update(ScrollTextLayer* layer);
static void on_text_scroll_timer(void *data);
static void reset_scroll_pos(void *data);
static uint16_t get_current_not_scrolled(ScrollTextLayer* layer, int16_t *current_offset);


ScrollTextLayer* scroll_text_layer_create(GRect rect) {
    
    ScrollTextLayer* stl = malloc(sizeof(ScrollTextLayer));
    
    stl->scroll_timer = NULL;
    stl->scroll_delay = 0;
    stl->scroll_interval = 0;
    stl->scroll_offset = 0;
    stl->hidden = false;
    
    stl->scroll_layer = scroll_layer_create(rect);
    GRect max_text_bounds = GRect(PADDING_X, PADDING_Y, rect.size.w - (PADDING_X * 2), MAX_HEIGHT);
    stl->text_layer = text_layer_create(max_text_bounds);
    
    scroll_layer_add_child(stl->scroll_layer, text_layer_get_layer(stl->text_layer));
    
    return stl;
}


ScrollTextLayer* scroll_text_layer_create_fullscreen(Window* window) {
    
    return scroll_text_layer_create(layer_get_bounds(window_get_root_layer(window)));
}


void scroll_text_layer_destroy(ScrollTextLayer* layer) {
    
    if (NULL == layer) {
        return;
    }
    
    scroll_text_layer_autoscroll_stop(layer);
    text_layer_destroy(layer->text_layer);
    scroll_layer_destroy(layer->scroll_layer);
    
    free(layer);
}


void scroll_text_layer_add_to_window(ScrollTextLayer* layer, Window* window) {
    
    if (NULL == layer || NULL == window) {
        return;
    }
    
    scroll_text_layer_update(layer);
    scroll_layer_set_click_config_onto_window(layer->scroll_layer, window);
    layer_add_child(window_get_root_layer(window), scroll_layer_get_layer(layer->scroll_layer));
}


void scroll_text_layer_set_text(ScrollTextLayer* layer, const char* text) {
    
    if (NULL == layer) {
        return;
    }
    
    text_layer_set_text(layer->text_layer, text);
    
    scroll_text_layer_update(layer);
}


void scroll_text_layer_set_font(ScrollTextLayer* layer, GFont font) {
    
    if (NULL == layer) {
        return;
    }
    
    text_layer_set_font(layer->text_layer, font);
    
    scroll_text_layer_update(layer);
}



void scroll_text_layer_set_system_font(ScrollTextLayer *layer, const char *font) {
    
    if (NULL == layer) {
        return;
    }
    
    scroll_text_layer_set_font(layer, fonts_get_system_font(font));
}


void scroll_text_layer_set_text_color(ScrollTextLayer *layer, GColor color) {
    
    if (NULL == layer) {
        return;
    }
    
    text_layer_set_text_color(layer->text_layer, color);
}


void scroll_text_layer_set_callbacks(ScrollTextLayer *layer, ScrollLayerCallbacks callbacks) {
    
    if (NULL == layer) {
        return;
    }
    
    scroll_layer_set_callbacks(layer->scroll_layer, callbacks);
}


void scroll_text_layer_set_background_color(ScrollTextLayer *layer, GColor color) {
    
    if (NULL == layer) {
        return;
    }
    
    text_layer_set_background_color(layer->text_layer, color);
}


void scroll_text_layer_set_text_alignment(ScrollTextLayer *layer, GTextAlignment alignment) {
    
    if (NULL == layer) {
        return;
    }
    
    text_layer_set_text_alignment(layer->text_layer, alignment);
}


void scroll_text_layer_autoscroll_start(ScrollTextLayer* layer, uint16_t delay,
                                   uint16_t interval, uint16_t offset) {
    
    if (NULL == layer) {
        return;
    }
    
    if (layer->scroll_timer) {
        APP_LOG(APP_LOG_LEVEL_WARNING, "Scroll timer already registered!");
        
        return;
    }

    GRect bounds = layer_get_bounds(scroll_layer_get_layer(layer->scroll_layer));
    if(interval > 0 && offset > 0 && get_current_not_scrolled(layer, NULL) > bounds.size.h) {
        
        layer->scroll_interval = interval;
        layer->scroll_offset = offset;
        layer->scroll_delay = delay;

        if (!layer->hidden) {
            layer->scroll_timer = app_timer_register(delay, on_text_scroll_timer, layer);
        }
    }
}


void scroll_text_layer_autoscroll_stop(ScrollTextLayer* layer) {
    
    if (layer && layer->scroll_timer) {
        
        app_timer_cancel(layer->scroll_timer);
        layer->scroll_timer = NULL;
    }
}


void scroll_text_layer_set_hidden(ScrollTextLayer* layer, bool hidden) {
    
    if (NULL == layer) {
        return;
    }
    
    if (hidden == layer->hidden) {
        return;
    }
    
    layer->hidden = hidden;
    
    layer_set_hidden((Layer*)layer->text_layer, hidden);
    layer_set_hidden((Layer*)layer->scroll_layer, hidden);
    
    if (hidden) {
        if (layer->scroll_timer) {
            app_timer_cancel(layer->scroll_timer);
            layer->scroll_timer = NULL;
        }
    } else {
        scroll_layer_set_content_offset(layer->scroll_layer, GPointZero, false);
        scroll_text_layer_autoscroll_start(layer, layer->scroll_delay,
                                      layer->scroll_interval, layer->scroll_offset);
    }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static void scroll_text_layer_update(ScrollTextLayer* layer) {
    
    //APP_LOG(APP_LOG_LEVEL_ERROR, "UPDATE\n");
    
    GSize max_size = text_layer_get_content_size(layer->text_layer);
    //text_layer_set_size(layer->text_layer, max_size);
    GRect bounds = layer_get_bounds(scroll_layer_get_layer(layer->scroll_layer));
    scroll_layer_set_content_size(layer->scroll_layer, GSize(bounds.size.w, max_size.h + (PADDING_Y * 3)));
}



static uint16_t get_current_not_scrolled(ScrollTextLayer* layer, int16_t *current_offset) {
    
    GPoint p = scroll_layer_get_content_offset(layer->scroll_layer);
    GSize sz = scroll_layer_get_content_size(layer->scroll_layer);
    
    if(current_offset) {
        *current_offset = p.y;
    }
    
    return sz.h + p.y;
}


static void reset_scroll_pos(void *data) {
    
    ScrollTextLayer* layer = (ScrollTextLayer*) data;
    
    scroll_layer_set_content_offset(layer->scroll_layer, GPointZero, false);
    scroll_text_layer_update(layer);
    
    layer->scroll_timer = app_timer_register(layer->scroll_delay, on_text_scroll_timer, data);
}


static void on_text_scroll_timer(void *data) {
    
    ScrollTextLayer* layer = (ScrollTextLayer*) data;
    int16_t curr_offset;
    
    GRect bounds = layer_get_bounds(scroll_layer_get_layer(layer->scroll_layer));
    if(get_current_not_scrolled(layer, &curr_offset) > bounds.size.h) {
        
        scroll_layer_set_content_offset(layer->scroll_layer,
                                        (GPoint) {.x = 0, .y = curr_offset - layer->scroll_offset},
                                        false);
        scroll_text_layer_update(layer);
        
        layer->scroll_timer = app_timer_register(layer->scroll_interval, on_text_scroll_timer, data);
    } else {
        layer->scroll_timer = app_timer_register(layer->scroll_delay, reset_scroll_pos, data);
    }
}
