#include "adapter.h"
#include <Arduino.h>

uint8_t pin_mode_to_arduino(pin_mode pin_mode);
uint8_t voltage_state_to_arduino(voltage_state state);
uint8_t pin_state_to_arduino(pin_state state);

void setup_pin(int pin_index, pin_mode pin_mode) {
  auto pin_mode_arduino = pin_mode_to_arduino(pin_mode);
  pinMode(pin_index, pin_mode_arduino);
}

void attach_interrupt(int pin, void (*isr)(), voltage_state state) {
  auto state_arduino = voltage_state_to_arduino(state);
  attachInterrupt(digitalPinToInterrupt(pin), isr, state_arduino);
}

void change_pin_state(int pin, pin_state state) {
  auto state_arduino = pin_state_to_arduino(state);
  digitalWrite(pin, state_arduino);
}

unsigned long get_millis_from_start() { return ::millis(); }

uint8_t pin_mode_to_arduino(pin_mode pin_mode) {
  switch (pin_mode) {
  case pin_mode::INPUT_PULLUP_MODE:
    return INPUT_PULLUP;
    break;
  case pin_mode::OUTPUT_MODE:
    return OUTPUT;
    break;
  default:
    return INPUT;
    break;
  }
}

uint8_t voltage_state_to_arduino(voltage_state state) {
  switch (state) {
  case voltage_state::RISING_STATE:
    return RISING;
    break;
  case voltage_state::FALLING_STATE:
    return FALLING;
    break;
  case voltage_state::CHANGE_STATE:
    return CHANGE;
    break;
  default:
    return RISING;
    break;
  }
}

uint8_t pin_state_to_arduino(pin_state state) {
  switch (state) {
  case pin_state::HIGH_STATE:
    return HIGH;
    break;
  case pin_state::LOW_STATE:
    return LOW;
    break;
  default:
    return LOW;
    break;
  }
}