#include <pebble.h>
#include "session_window.h"
#include "setup_window.h"

static Window *active_window;
static Tuple *id_tuple, *name_tuple, *set_tuple, *rep_tuple, *weight_tuple, *timer_tuple;

uint32_t id;
char *name;
int32_t set;
int32_t rep;
int32_t weight;

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
  // A new message has been successfully received
  id_tuple = dict_find(iter, MESSAGE_KEY_EXERCISE_ID);
  name_tuple = dict_find(iter, MESSAGE_KEY_EXERCISE_NAME);
  set_tuple = dict_find(iter, MESSAGE_KEY_EXERCISE_SET);
  rep_tuple = dict_find(iter, MESSAGE_KEY_EXERCISE_REP);
  weight_tuple = dict_find(iter, MESSAGE_KEY_EXERCISE_WEIGHT);
  timer_tuple = dict_find(iter, MESSAGE_KEY_EXERCISE_REST_TIMER);
  
  if (session_window_get_window() == NULL){
    // Create new session window and set active
    session_window_create();
    active_window = session_window_get_window();
    window_stack_push(active_window, true);
    
    // Remove and destroy setup window
    window_stack_remove(setup_window_get_window(), false);
    setup_window_destroy();
  }
  
  id = id_tuple->value->uint32;
  name = name_tuple->value->cstring;
  set = set_tuple->value->int32;
  rep = rep_tuple->value->int32;
  weight = weight_tuple->value->int32;
  
  APP_LOG(APP_LOG_LEVEL_INFO, "Received id: %d, name: %s, set: %d, rep: %d, weight: %d",
          (int) id, name, (int) set, (int) rep, (int) weight);
  
  session_window_update_view(name, set, rep, weight);
}

// Request the session data from the phone
static void send_request_current_session() {
  DictionaryIterator *req;

  AppMessageResult result = app_message_outbox_begin(&req);
  if (result == APP_MSG_OK) {
    // Prepare the dict
    int value = 0;
    dict_write_int(req, MESSAGE_KEY_REQUEST_DATA, &value, sizeof(int), true);
    
    // Send the message
    result = app_message_outbox_send();
    if(result != APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
    }
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int) result);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  // A message was received, but had to be dropped
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped. Reason: %d", (int)reason);
}

static void outbox_failed_callback(DictionaryIterator *iter,
                                      AppMessageResult reason, void *context) {
  // The message just sent failed to be delivered
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message send failed. Reason: %d", (int)reason);
}

void handle_init(void) {
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  
  app_message_register_outbox_failed(outbox_failed_callback);
  
  // Send query message for active exercise
  send_request_current_session();
  
  setup_window_create();
  active_window = setup_window_get_window();
  window_stack_push(active_window, true);
}

void handle_deinit(void) {
  window_destroy(active_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
