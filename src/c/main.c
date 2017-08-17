#include <pebble.h>
#include "session_window.h"

void handle_init(void) {
  session_window_create();
  window_stack_push(session_window_get_window(), true);
}

void handle_deinit(void) {
  session_window_destroy();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
