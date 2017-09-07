#include "session_window.h"

Window *session_window;
ActionBarLayer *action_bar;
StatusBarLayer *status_bar;
TextLayer *name_text_layer, *set_text_layer, *rep_text_layer, *weight_text_layer, *timer_text_layer;

GBitmap *bitmap_icon_up, *bitmap_icon_down, *bitmap_icon_more, *bitmap_icon_check, *bitmap_icon_dismiss;

static struct SessionClickHandler click_handler;

void select_text_layer(TextLayer *text_layer){
  // Unselect the previous text
  text_layer_set_font(rep_text_layer, fonts_get_system_font(FONT_ENTRY));
  text_layer_set_font(weight_text_layer, fonts_get_system_font(FONT_ENTRY));
  
  // Select the new text_layer
  if (text_layer != NULL) {
    text_layer_set_font(text_layer, fonts_get_system_font(FONT_ENTRY_SELECTED));
  }
}

void session_window_update_view(char* name, int set, int rep, int weight) {
  session_window_update_name(name);
  session_window_update_set(set);
  session_window_update_rep(rep);
  session_window_update_weight(weight);
}

void session_window_update_name(char* name) {
  text_layer_set_text(name_text_layer, name);
}

void session_window_update_set(int set) {
  static char set_string[12];
  snprintf(set_string, sizeof(set_string), "Set %d", set);
  
  text_layer_set_text(set_text_layer, set_string);
}

void session_window_update_rep(int rep) {
  static char rep_string[14];
  snprintf(rep_string, sizeof(rep_string), "Reps: %d", rep);
  
  text_layer_set_text(rep_text_layer, rep_string);
}

void session_window_update_weight(int weight) {
  // TODO print weight with 2 point decimal
  static char weight_string[15];
  snprintf(weight_string, sizeof(weight_string), "Weight: %d", weight);
  
  text_layer_set_text(weight_text_layer, weight_string);
}

void session_window_start_set(bool start_now) {
  if (start_now) {
    text_layer_set_text(timer_text_layer, "Start set now!");
  } else {
    text_layer_set_text(timer_text_layer, NULL);
  }
}

void complete_set_config_provider() {
  window_single_click_subscribe(BUTTON_ID_UP, click_handler.complete_set);
  window_single_click_subscribe(BUTTON_ID_SELECT, setup_select_rep_view);
  
  window_multi_click_subscribe(BUTTON_ID_UP, 2, 0, 0, true, click_handler.previous_exercise);
  window_multi_click_subscribe(BUTTON_ID_DOWN, 2, 0, 0, true, click_handler.next_exercise);
}

void select_rep_config_provider() {
  window_single_click_subscribe(BUTTON_ID_UP, click_handler.incr_rep);
  window_single_click_subscribe(BUTTON_ID_SELECT, setup_select_weight_view);
  window_single_click_subscribe(BUTTON_ID_DOWN, click_handler.decr_rep);
}

void select_weight_config_provider() {
  window_single_click_subscribe(BUTTON_ID_UP, click_handler.incr_weight);
  window_single_click_subscribe(BUTTON_ID_SELECT, setup_complete_set_view);
  window_single_click_subscribe(BUTTON_ID_DOWN, click_handler.decr_weight);
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
  action_bar_layer_clear_icon(action_bar, BUTTON_ID_DOWN);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, bitmap_icon_more);
}

TextLayer* init_text_layer(GRect root_bound, int y, const char *font_key, GTextAlignment align) {
  TextLayer *text_layer = text_layer_create(GRect(0, y, root_bound.size.w - ACTION_BAR_WIDTH, 30));
  text_layer_set_font(text_layer, fonts_get_system_font(font_key));
  text_layer_set_overflow_mode(text_layer, GTextOverflowModeTrailingEllipsis);
  text_layer_set_text_alignment(text_layer, align);
  return text_layer;
}

void session_window_load(Window *window) {
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Setup status bar
  status_bar = status_bar_layer_create();
  status_bar_layer_set_colors(status_bar, GColorWhite, GColorBlack);
  int16_t width = bounds.size.w - ACTION_BAR_WIDTH;
  GRect frame = GRect(0, 0, width, STATUS_BAR_LAYER_HEIGHT);
  layer_set_frame(status_bar_layer_get_layer(status_bar), frame);
  layer_add_child(window_layer, status_bar_layer_get_layer(status_bar));
  
  // Create the session info text
  name_text_layer = init_text_layer(bounds, 35, FONT_HEADER, GTextAlignmentCenter);
  set_text_layer = init_text_layer(bounds, 65, FONT_ENTRY, GTextAlignmentCenter);
  rep_text_layer = init_text_layer(bounds, 85, FONT_ENTRY, GTextAlignmentCenter);
  weight_text_layer = init_text_layer(bounds, 105, FONT_ENTRY, GTextAlignmentCenter);
  timer_text_layer = init_text_layer(bounds, 125, FONT_ENTRY, GTextAlignmentCenter);
  
  layer_add_child(window_layer, text_layer_get_layer(name_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(set_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(rep_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(weight_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(timer_text_layer));

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
  status_bar_layer_destroy(status_bar);
  text_layer_destroy(name_text_layer);
  text_layer_destroy(set_text_layer);
  text_layer_destroy(rep_text_layer);
  text_layer_destroy(weight_text_layer);
  text_layer_destroy(timer_text_layer);
  action_bar_layer_destroy(action_bar);
  gbitmap_destroy(bitmap_icon_up);
  gbitmap_destroy(bitmap_icon_down);
  gbitmap_destroy(bitmap_icon_more);
  gbitmap_destroy(bitmap_icon_check);
  gbitmap_destroy(bitmap_icon_dismiss);
}

void session_window_create(struct SessionClickHandler session_click_handler) {
  session_window = window_create();
  window_set_window_handlers(session_window, (WindowHandlers) {
    .load = session_window_load,
    .unload = session_window_unload
  });
  click_handler = session_click_handler;
}

void session_window_destroy() {
  window_destroy(session_window);
}

Window *session_window_get_window() {
  return session_window;
}

