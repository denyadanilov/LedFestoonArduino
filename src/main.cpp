#include "adapter.h"
#include "festoon_sequences.h"
#include "loop_logger.h"
#include <cstdio>

void setup() {
  initialize_logger();
  setup_pin(WHITE_LED_PIN, pin_mode::OUTPUT_MODE);
  setup_pin(BLUE_LED_PIN, pin_mode::OUTPUT_MODE);
  setup_pin(RED_LED_PIN, pin_mode::OUTPUT_MODE);
  setup_pin(YELLOW_LED_PIN, pin_mode::OUTPUT_MODE);
  setup_pin(GREEN_LED_PIN, pin_mode::OUTPUT_MODE);
  setup_pin(BUTTON_PIN, pin_mode::INPUT_PULLUP_MODE);
  attach_interrupt(BUTTON_PIN, on_button_clicked, voltage_state::RISING_STATE);
}

void loop() {
  log_loop_time([]() {
    auto time = get_millis_from_start();
    try_change_sequence(time);
    updated_sequence(time);
  });
}
