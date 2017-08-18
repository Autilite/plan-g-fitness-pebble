#pragma once

#define FONT_HEADER FONT_KEY_GOTHIC_24_BOLD
#define FONT_ENTRY FONT_KEY_GOTHIC_18
#define FONT_ENTRY_SELECTED FONT_KEY_GOTHIC_18_BOLD

void session_window_create();
void session_window_destroy();
Window *session_window_get_window();

void setup_select_weight_view();
void setup_select_rep_view();
void setup_complete_set_view();

void session_window_update_view(char* name, int set, int rep, int weight);