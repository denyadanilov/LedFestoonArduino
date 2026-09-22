#pragma once

#include "festoon_sequence_item.h"
#include <stdint.h>

#define WHITE_LED_PIN 5
#define BLUE_LED_PIN 6
#define RED_LED_PIN 7
#define YELLOW_LED_PIN 17
#define GREEN_LED_PIN 16
#define BUTTON_PIN 18
#define DEBOUNCE_DELAY 25

typedef struct {
  sequence_item *items;
  uint8_t size;
} sequence;

bool try_change_sequence(unsigned long time);
uint8_t get_next_sequence_index(uint8_t sequence_index, uint8_t sequence_num);
uint8_t get_next_item_index(uint8_t item_index, uint8_t sequence_size);
void updated_sequence(unsigned long time);
void on_button_clicked();