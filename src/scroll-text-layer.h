/*
 
 Scroll Text Layer 1.0.0
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
 
 src/scroll-text-layer.h
 
 */

#pragma once

#include <pebble.h>

struct ScrollTextLayer;
typedef struct ScrollTextLayer ScrollTextLayer;

ScrollTextLayer* scroll_text_layer_create(GRect rect);
ScrollTextLayer* scroll_text_layer_create_fullscreen(Window* window);
void scroll_text_layer_set_callbacks(ScrollTextLayer *layer, ScrollLayerCallbacks callbacks);
void scroll_text_layer_add_to_window(ScrollTextLayer* layer, Window* window);
void scroll_text_layer_destroy(ScrollTextLayer* layer);

void scroll_text_layer_set_hidden(ScrollTextLayer*, bool hidden);
void scroll_text_layer_set_background_color(ScrollTextLayer *layer, GColor color);
void scroll_text_layer_set_text(ScrollTextLayer* layer, const char* text);
void scroll_text_layer_set_text_color(ScrollTextLayer *layer, GColor color);
void scroll_text_layer_set_text_alignment(ScrollTextLayer *layer, GTextAlignment alignment);
void scroll_text_layer_set_font(ScrollTextLayer*, GFont font);
void scroll_text_layer_set_system_font(ScrollTextLayer *layer, const char *font);
void scroll_text_layer_autoscroll_start(ScrollTextLayer* layer, uint16_t delay, uint16_t interval, uint16_t offset);
void scroll_text_layer_autoscroll_stop(ScrollTextLayer* layer);






