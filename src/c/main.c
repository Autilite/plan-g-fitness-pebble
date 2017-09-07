#include <pebble.h>
#include "session_window.h"
#include "setup_window.h"

#define MIN_REP 0
#define MAX_REP 50
#define INCREMENT_REP 1

#define MIN_WEIGHT 0
#define MAX_WEIGHT 100000
#define INCREMENT_WEIGHT 25

static Window *active_window;
static Tuple *id_tuple, *name_tuple, *set_tuple, *rep_tuple, *weight_tuple, *timer_tuple;
static AppTimer *timer;

uint32_t id;
char *name;
int32_t set;
int32_t rep;
int32_t weight;

static void send_completed_set_message() {
  DictionaryIterator *dict;

  AppMessageResult result = app_message_outbox_begin(&dict);
  if (result == APP_MSG_OK) {
    // Set the dictionary values
    dict_write_uint32(dict, MESSAGE_KEY_EXERCISE_ID, id);
    dict_write_int32(dict, MESSAGE_KEY_EXERCISE_SET, set);
    dict_write_int32(dict, MESSAGE_KEY_EXERCISE_REP, rep);
    dict_write_int32(dict, MESSAGE_KEY_EXERCISE_WEIGHT, weight);
    
    // Send the message
    result = app_message_outbox_send();
    if(result != APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
    }
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int) result);
  }
}

void complete_set_handler() {
  send_completed_set_message();
}

void increase_rep_handler() {
  int32_t new_rep = rep + INCREMENT_REP;
  if (new_rep <= MAX_REP) {
    rep = new_rep;
    session_window_update_rep(rep);
  }
}

void decrease_rep_handler() {
  int32_t new_rep = rep - INCREMENT_REP;
  if (new_rep >= MIN_REP) {
    rep = new_rep;
    session_window_update_rep(rep);
  }
}

void increase_weight_handler() {
  int32_t new_weight = weight + INCREMENT_WEIGHT;
  if (new_weight <= MAX_WEIGHT) {
    weight = new_weight;
    session_window_update_weight(weight);
  }
}

void decrease_weight_handler() {
  int32_t new_weight = weight - INCREMENT_WEIGHT;
  if (new_weight >= MIN_WEIGHT) {
    weight = new_weight;
    session_window_update_weight(weight);
  }
}

static void send_request_select_previous_exercise() {
  DictionaryIterator *req;

  AppMessageResult result = app_message_outbox_begin(&req);
  if (result == APP_MSG_OK) {
    // Prepare the dict
    int value = 0;
    dict_write_int(req, MESSAGE_KEY_REQUEST_CHANGE_PREVIOUS_EXERCISE, &value, sizeof(int), true);
    
    // Send the message
    result = app_message_outbox_send();
    if(result != APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
    }
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int) result);
  }
}

static void send_request_select_next_exercise() {
  DictionaryIterator *req;

  AppMessageResult result = app_message_outbox_begin(&req);
  if (result == APP_MSG_OK) {
    // Prepare the dict
    int value = 0;
    dict_write_int(req, MESSAGE_KEY_REQUEST_CHANGE_NEXT_EXERCISE, &value, sizeof(int), true);
    
    // Send the message
    result = app_message_outbox_send();
    if(result != APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
    }
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int) result);
  }
}

void notify_start_set_callback(void *data) {
  session_window_start_set(true);
  vibes_double_pulse();
}

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
  // A new message has been successfully received
  id_tuple = dict_find(iter, MESSAGE_KEY_EXERCISE_ID);
  name_tuple = dict_find(iter, MESSAGE_KEY_EXERCISE_NAME);
  set_tuple = dict_find(iter, MESSAGE_KEY_EXERCISE_SET);
  rep_tuple = dict_find(iter, MESSAGE_KEY_EXERCISE_REP);
  weight_tuple = dict_find(iter, MESSAGE_KEY_EXERCISE_WEIGHT);
  timer_tuple = dict_find(iter, MESSAGE_KEY_EXERCISE_REST_TIMER);
  
  // Create new session window and set active if it does not already exist
  if (session_window_get_window() == NULL){
    session_window_create((struct SessionClickHandler) {
      .incr_rep = increase_rep_handler,
      .decr_rep = decrease_rep_handler,
      .incr_weight = increase_weight_handler,
      .decr_weight = decrease_weight_handler,
      .complete_set = complete_set_handler,
      .previous_exercise = send_request_select_previous_exercise,
      .next_exercise = send_request_select_next_exercise
    });
    active_window = session_window_get_window();
    window_stack_push(active_window, true);
    
    // Remove and destroy setup window
    window_stack_remove(setup_window_get_window(), false);
    setup_window_destroy();
  }
  
  // Check if all the exercise data is received
  if (id_tuple && name_tuple && set_tuple && rep_tuple && weight_tuple) {
    id = id_tuple->value->uint32;
    name = name_tuple->value->cstring;
    set = set_tuple->value->int32;
    rep = rep_tuple->value->int32;
    weight = weight_tuple->value->int32;

    APP_LOG(APP_LOG_LEVEL_INFO, "Received id: %d, name: %s, set: %d, rep: %d, weight: %d",
            (int) id, name, (int) set, (int) rep, (int) weight);

    session_window_update_view(name, set, rep, weight);
  }
  
  if (timer_tuple) {
    uint32_t rest_time_ms = timer_tuple->value->uint32;
    
    if (rest_time_ms > 0) {
      if (!app_timer_reschedule(timer, rest_time_ms)) {
        timer = app_timer_register(rest_time_ms, notify_start_set_callback, NULL);
        APP_LOG(APP_LOG_LEVEL_INFO, "Created new timer: %d milliseconds", (int) rest_time_ms);
      } else {
        APP_LOG(APP_LOG_LEVEL_INFO, "Rescheduled timer: %d milliseconds", (int) rest_time_ms);
      }
      
      // We just received a new timer. This could signal that the previous set has been completed
      // so clear the text that says to start the next set
      session_window_start_set(false);
    }
  }
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
