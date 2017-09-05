#include <pebble.h>
#pragma once

#define SELECT_EXERCISE "Start a program and select an exercise on your Android phone"

void setup_window_create();
void setup_window_destroy();
Window *setup_window_get_window();