#include <pebble.h>
#include "session_window.h"

Window *session_window;
ActionBarLayer *action_bar;
TextLayer *name_text_layer, *set_text_layer, *rep_text_layer, *weight_text_layer;

GBitmap *bitmap_icon_up, *bitmap_icon_down, *bitmap_icon_more, *bitmap_icon_check, *bitmap_icon_dismiss;

void select_text_layer(TextLayer *text_layer){
  // Unselect the previous text
  text_layer_set_font(rep_text_layer, fonts_get_system_font(FONT_ENTRY));
  text_layer_set_font(weight_text_layer, fonts_get_system_font(FONT_ENTRY));
  
  // Select the new text_layer
  if (text_layer != NULL) {
    text_layer_set_font(text_layer, fonts_get_system_font(FONT_ENTRY_SELECTED));
  }
}

void complete_set_config_provider() {
  window_single_click_subscribe(BUTTON_ID_SELECT, setup_select_rep_view);
}

void select_rep_config_provider() {
  window_single_click_subscribe(BUTTON_ID_SELECT, setup_select_weight_view);
}

void select_weight_config_provider() {
  window_single_click_subscribe(BUTTON_ID_SELECT, setup_complete_set_view);
}

void setup_select_weight_view() {
  select_text_layer(weight_text_layer);
  action_bar_layer_set_click_config_provider(action_bar, select_weight_config_provider);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, bitmap_icon_up);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, bitmap_icon_down);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, bitmap_icon_more);
}

void setup_select_rep_view() {
  select_text_layer(rep_text_layer);
  action_bar_layer_set_click_config_provider(action_bar, select_rep_config_provider);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, bitmap_icon_up);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, bitmap_icon_down);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, bitmap_icon_more);
}

void setup_complete_set_view() {
  select_text_layer(NULL);
  action_bar_layer_set_click_config_provider(action_bar, complete_set_config_provider);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, bitmap_icon_check);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, bitmap_icon_dismiss);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, bitmap_icon_more);
}

TextLayer* init_text_layer(GRect root_bound, int y, char *text, const char *font_key, GTextAlignment align) {
  TextLayer *text_layer = text_layer_create(GRect(0, y, root_bound.size.w - ACTION_BAR_WIDTH, 30));
  text_layer_set_text(text_layer, text);
  text_layer_set_font(text_layer, fonts_get_system_font(font_key));
  text_layer_set_overflow_mode(text_layer, GTextOverflowModeTrailingEllipsis);
  text_layer_set_text_alignment(text_layer, align);
  return text_layer;
}

void session_window_load(Window *window) {
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create the session info text
  name_text_layer = init_text_layer(bounds, 35, "Squats", FONT_HEADER, GTextAlignmentCenter);
  set_text_layer = init_text_layer(bounds, 65, "Set 1", FONT_ENTRY, GTextAlignmentCenter);
  rep_text_layer = init_text_layer(bounds, 85, "Reps: 5", FONT_ENTRY, GTextAlignmentCenter);
  weight_text_layer = init_text_layer(bounds, 105, "Weight: 150", FONT_ENTRY, GTextAlignmentCenter);
  
  layer_add_child(window_layer, text_layer_get_layer(name_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(set_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(rep_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(weight_text_layer));

  // Create the action bar
  action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(action_bar, window);
  
  // Setup action bar resources
  bitmap_icon_up = gbitmap_create_with_resource(RESOURCE_ID_ICON_UP);
  bitmap_icon_down = gbitmap_create_with_resource(RESOURCE_ID_ICON_DOWN);
  bitmap_icon_more = gbitmap_create_with_resource(RESOURCE_ID_ICON_ELLIPSIS);
  bitmap_icon_check = gbitmap_create_with_resource(RESOURCE_ID_ICON_CHECK);
  bitmap_icon_dismiss = gbitmap_create_with_resource(RESOURCE_ID_ICON_CROSS);
  
  // Setup view with complete/fail actions for the selected exercise
  setup_complete_set_view();
}

void session_window_unload(Window *window) {
  text_layer_destroy(name_text_layer);
  text_layer_destroy(set_text_layer);
  text_layer_destroy(rep_text_layer);
  text_layer_destroy(weight_text_layer);
  action_bar_layer_destroy(action_bar);
  gbitmap_destroy(bitmap_icon_up);
  gbitmap_destroy(bitmap_icon_down);
  gbitmap_destroy(bitmap_icon_more);
  gbitmap_destroy(bitmap_icon_check);
  gbitmap_destroy(bitmap_icon_dismiss);
}

void session_window_create() {
  session_window = window_create();
  window_set_window_handlers(session_window, (WindowHandlers) {
    .load = session_window_load,
    .unload = session_window_unload
  });
}

void session_window_destroy() {
  window_destroy(session_window);
}

Window *session_window_get_window() {
  return session_window;
}