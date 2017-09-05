#include "setup_window.h"

Window *setup_window;
TextLayer *text_layer;

void setup_window_load(Window *window) {
  Layer *root_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root_layer);
  
  text_layer = text_layer_create(bounds);
  text_layer_set_text(text_layer, SELECT_EXERCISE);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(root_layer, text_layer_get_layer(text_layer));
}

void setup_window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

void setup_window_create() {
  setup_window = window_create();
  window_set_window_handlers(setup_window, (WindowHandlers) {
    .load = setup_window_load,
    .unload = setup_window_unload
  });
}

void setup_window_destroy() {
  window_destroy(setup_window);
}

Window *setup_window_get_window() {
  return setup_window;
}
