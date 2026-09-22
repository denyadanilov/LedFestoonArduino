#include "festoon_sequence_item.h"
#include "adapter.h"

void reset_sequence_item(sequence_item &current_item) {
  change_pin_state(current_item.led_pin, pin_state::LOW_STATE);
  current_item.end_millis = 0;
}

void activate_sequence_item(sequence_item &current_item) {
  change_pin_state(current_item.led_pin, pin_state::HIGH_STATE);
  current_item.end_millis = get_millis_from_start() + current_item.delay;
}

bool is_item_disabled(const sequence_item &item) {
  return item.end_millis == 0;
}
bool is_item_completed(const sequence_item &item, unsigned long time) {
  return item.end_millis < time;
}