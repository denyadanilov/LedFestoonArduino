#pragma once

#include <stdint.h>

typedef struct {
  uint8_t delay;
  uint8_t led_pin;
  unsigned long end_millis;
} sequence_item;

void activate_sequence_item(sequence_item &current_item);
void reset_sequence_item(sequence_item &current_item);
bool is_item_disabled(const sequence_item &item);
bool is_item_completed(const sequence_item &item, unsigned long time);