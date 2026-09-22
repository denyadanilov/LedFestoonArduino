#pragma once

enum class pin_mode { NONE, OUTPUT_MODE, INPUT_PULLUP_MODE };
enum class voltage_state { RISING_STATE, FALLING_STATE, CHANGE_STATE };
enum class pin_state { LOW_STATE, HIGH_STATE };

void setup_pin(int pin_index, pin_mode pin_mode);
void attach_interrupt(int pin, void (*isr)(), voltage_state state);
void change_pin_state(int pin, pin_state state);
unsigned long get_millis_from_start();