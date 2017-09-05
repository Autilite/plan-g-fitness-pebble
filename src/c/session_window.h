#include <pebble.h>
#pragma once

#define FONT_HEADER FONT_KEY_GOTHIC_24_BOLD
#define FONT_ENTRY FONT_KEY_GOTHIC_18
#define FONT_ENTRY_SELECTED FONT_KEY_GOTHIC_18_BOLD

#ifndef SESSIONCLICKHANDLER_HEADER
#define SESSIONCLICKHANDLER_HEADER
struct SessionClickHandler {
    ClickHandler complete_set;
    ClickHandler incr_rep;
    ClickHandler decr_rep;
    ClickHandler incr_weight;
    ClickHandler decr_weight;
};
#endif

void session_window_create(struct SessionClickHandler session_click_handler);
void session_window_destroy();
Window *session_window_get_window();

void setup_select_weight_view();
void setup_select_rep_view();
void setup_complete_set_view();

void session_window_update_view(char* name, int set, int rep, int weight);
void session_window_update_name(char* name);
void session_window_update_set(int set);
void session_window_update_rep(int rep);
void session_window_update_weight(int weight);
void session_window_start_set(bool start_now);