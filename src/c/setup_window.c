#include <pebble.h>
#include "setup_window.h"

Window *select_window;
TextLayer *text_layer;

void select_window_load(Window *window) {
  Layer *root_layer = window_get_root_layer(window);
  
  text_layer = text_layer_create(layer_get_bounds(root_layer));
  text_layer_set_text(text_layer, SELECT_EXERCISE);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(root_layer, text_layer_get_layer(text_layer));
}

void select_window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

void select_window_create() {
  select_window = window_create();
  window_set_window_handlers(select_window, (WindowHandlers) {
    .load = select_window_load,
    .unload = select_window_unload
  });
}

void select_window_destroy() {
  window_destroy(select_window);
}

Window *select_window_get_window() {
  return select_window;
}