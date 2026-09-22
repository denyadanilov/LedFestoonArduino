#include "festoon_sequences.h"
#include "adapter.h"
#include "festoon_sequence_item.h"

const uint8_t sequences_number = 3;

volatile bool button_clicked_ = false;
unsigned long last_button_click_time_ = 0;
int current_sequence_index_ = 0;
int current_item_index_ = 0;

sequence first_sequence_{
    .items = (sequence_item[]){sequence_item{100, WHITE_LED_PIN, 0},
                               sequence_item{100, BLUE_LED_PIN, 0},
                               sequence_item{100, RED_LED_PIN, 0},
                               sequence_item{100, YELLOW_LED_PIN, 0},
                               sequence_item{100, GREEN_LED_PIN, 0}},
    .size = 5};

sequence second_sequence_{
    .items = (sequence_item[]){sequence_item{100, GREEN_LED_PIN, 0},
                               sequence_item{100, YELLOW_LED_PIN, 0},
                               sequence_item{100, RED_LED_PIN, 0},
                               sequence_item{100, BLUE_LED_PIN, 0},
                               sequence_item{100, WHITE_LED_PIN, 0}},
    .size = 5};

sequence third_sequence_{
    .items = (sequence_item[]){sequence_item{100, WHITE_LED_PIN, 0},
                               sequence_item{80, BLUE_LED_PIN, 0},
                               sequence_item{60, RED_LED_PIN, 0},
                               sequence_item{40, YELLOW_LED_PIN, 0},
                               sequence_item{20, GREEN_LED_PIN, 0},
                               sequence_item{40, YELLOW_LED_PIN, 0},
                               sequence_item{60, RED_LED_PIN, 0},
                               sequence_item{80, BLUE_LED_PIN, 0},
                               sequence_item{100, WHITE_LED_PIN, 0}},
    .size = 10};

sequence sequences[sequences_number] = {first_sequence_, second_sequence_,
                                        third_sequence_};

bool try_change_sequence(unsigned long time) {
  if (button_clicked_ && (time - last_button_click_time_ > DEBOUNCE_DELAY)) {
    auto current_sequence = &sequences[current_sequence_index_];
    auto current_item = &current_sequence->items[current_item_index_];

    reset_sequence_item(*current_item);

    current_sequence_index_ =
        get_next_sequence_index(current_sequence_index_, sequences_number);
    current_item_index_ = 0;

    last_button_click_time_ = time;

    button_clicked_ = false;

    return true;
  }

  button_clicked_ = false;
  return false;
}

void updated_sequence(unsigned long time) {
  auto current_sequence = &sequences[current_sequence_index_];
  auto current_item = &current_sequence->items[current_item_index_];

  if (is_item_disabled(*current_item)) {
    activate_sequence_item(*current_item);
  } else if (is_item_completed(*current_item, time)) {
    reset_sequence_item(*current_item);
    current_item_index_ =
        get_next_item_index(current_item_index_, current_sequence->size);
  }
}

uint8_t get_next_sequence_index(uint8_t sequence_index, uint8_t sequence_num) {
  if (sequence_index < sequence_num - 1) {
    return sequence_index + 1;
  }

  return 0;
}

uint8_t get_next_item_index(uint8_t item_index, uint8_t sequence_size) {
  if (item_index < sequence_size - 1) {
    return item_index + 1;
  }

  return 0;
}

void on_button_clicked() { button_clicked_ = true; }