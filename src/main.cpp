#include <Arduino.h>

#define WHITE_LED_PIN 5
#define BLUE_LED_PIN 6
#define RED_LED_PIN 7
#define YELLOW_LED_PIN 17
#define GREEN_LED_PIN 16
#define BUTTON_PIN 18
#define DEBOUNCE_DELAY 50
#define MONITOR_SPEED 115200

enum class pin_mode { NONE, OUTPUT_MODE, INPUT_PULLUP_MODE };
enum class voltage_state { RISING_STATE, FALLING_STATE, CHANGE_STATE };
enum class pin_state { LOW_STATE, HIGH_STATE };

typedef struct {
  uint8_t delay;
  uint8_t led_pin;
  unsigned long end_millis;
} sequence_item;

typedef struct {
  sequence_item *items;
  uint8_t size;
} sequence;

const uint8_t sequences_number = 3;

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

volatile bool button_clicked_ = false;
unsigned long last_button_click_time_ = 0;
int current_sequence_index_ = 0;
int current_item_index_ = 0;

void setup_pin(int pin_index, pin_mode pin_mode);
void attach_interrupt(int pin, void (*isr)(), voltage_state state);
void change_pin_state(int pin, pin_state state);
uint8_t pin_mode_to_arduino(pin_mode pin_mode);
uint8_t voltage_state_to_arduino(voltage_state state);
uint8_t pin_state_to_arduino(pin_state state);
uint8_t get_next_sequence_index(uint8_t sequence_index, uint8_t sequence_num);
uint8_t get_next_item_index(uint8_t item_index, uint8_t sequence_size);
void activate_sequence_item(sequence_item &current_item);
void reset_sequence_item(sequence_item &current_item);
bool is_item_disabled(const sequence_item &item);
bool is_item_completed(const sequence_item &item, unsigned long time);
void on_button_clicked();
bool try_change_sequence(sequence_item &current_item, unsigned long time);

void setup() {
  setup_pin(WHITE_LED_PIN, pin_mode::OUTPUT_MODE);
  setup_pin(BLUE_LED_PIN, pin_mode::OUTPUT_MODE);
  setup_pin(RED_LED_PIN, pin_mode::OUTPUT_MODE);
  setup_pin(YELLOW_LED_PIN, pin_mode::OUTPUT_MODE);
  setup_pin(GREEN_LED_PIN, pin_mode::OUTPUT_MODE);
  setup_pin(BUTTON_PIN, pin_mode::INPUT_PULLUP_MODE);
  attach_interrupt(BUTTON_PIN, on_button_clicked, voltage_state::RISING_STATE);
}

void loop() {
  auto time = millis();
  auto current_sequence = &sequences[current_sequence_index_];
  auto current_item = &current_sequence->items[current_item_index_];

  if (is_item_disabled(*current_item)) {
    activate_sequence_item(*current_item);
  } else if (is_item_completed(*current_item, time)) {
    reset_sequence_item(*current_item);
    current_item_index_ =
        get_next_item_index(current_item_index_, current_sequence->size);
  }

  try_change_sequence(*current_item, time);
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

void reset_sequence_item(sequence_item &current_item) {
  change_pin_state(current_item.led_pin, pin_state::LOW_STATE);
  current_item.end_millis = 0;
}

void activate_sequence_item(sequence_item &current_item) {
  change_pin_state(current_item.led_pin, pin_state::HIGH_STATE);
  current_item.end_millis = millis() + current_item.delay;
}

bool is_item_disabled(const sequence_item &item) {
  return item.end_millis == 0;
}
bool is_item_completed(const sequence_item &item, unsigned long time) {
  return item.end_millis < time;
}

bool try_change_sequence(sequence_item &current_item, unsigned long time) {
  if (button_clicked_ && (time - last_button_click_time_ > DEBOUNCE_DELAY)) {
    reset_sequence_item(current_item);

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

void on_button_clicked() { button_clicked_ = true; }

void setup_pin(int pin_index, pin_mode pin_mode) {
  auto pin_mode_arduino = pin_mode_to_arduino(pin_mode);
  pinMode(pin_index, pin_mode_arduino);
}

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

void attach_interrupt(int pin, void (*isr)(), voltage_state state) {
  auto state_arduino = voltage_state_to_arduino(state);
  attachInterrupt(digitalPinToInterrupt(pin), isr, state_arduino);
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

void change_pin_state(int pin, pin_state state) {
  auto state_arduino = pin_state_to_arduino(state);
  digitalWrite(pin, state_arduino);
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
