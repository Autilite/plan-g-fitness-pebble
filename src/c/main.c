#include <pebble.h>
#include "session_window.h"
#include "setup_window.h"

void handle_init(void) {
  setup_window_create();
  window_stack_push(setup_window_get_window(), true);
}

void handle_deinit(void) {
  setup_window_create();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
