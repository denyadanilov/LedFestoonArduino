#include <Arduino.h>

#define WHITE_LED_PIN 5
#define BLUE_LED_PIN 6
#define RED_LED_PIN 7
#define YELLOW_LED_PIN 17
#define GREEN_LED_PIN 16
#define BUTTON_PIN 18
#define DEBOUNCE_DELAY 50

enum class pin_mode { NONE, OUTPUT_MODE, INPUT_PULLUP_MODE };

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
uint8_t pin_mode_to_arduino(pin_mode pin_mode);
uint8_t get_next_sequence_index(uint8_t sequence_index, uint8_t sequence_num);
uint8_t get_next_item_index(uint8_t item_index, uint8_t sequence_size);
void reset_sequence_item(uint8_t sequence_index, uint8_t item_index);
bool is_item_disabled(const sequence_item &item);
bool is_item_completed(const sequence_item &item, unsigned long time);
void on_button_clicked();

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }

  setup_pin(WHITE_LED_PIN, pin_mode::OUTPUT_MODE);
  setup_pin(BLUE_LED_PIN, pin_mode::OUTPUT_MODE);
  setup_pin(RED_LED_PIN, pin_mode::OUTPUT_MODE);
  setup_pin(YELLOW_LED_PIN, pin_mode::OUTPUT_MODE);
  setup_pin(GREEN_LED_PIN, pin_mode::OUTPUT_MODE);
  setup_pin(BUTTON_PIN, pin_mode::INPUT_PULLUP_MODE);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), on_button_clicked, RISING);
}

void loop() {
  if (button_clicked_ &&
      (millis() - last_button_click_time_ > DEBOUNCE_DELAY)) {
    Serial.println("Button clicked " + String(last_button_click_time_));
  }

  button_clicked_ = false;
  last_button_click_time_ = millis();

  auto current_sequence = &sequences[current_sequence_index_];
  auto current_item = &current_sequence->items[current_item_index_];
  auto time = millis();

  if (is_item_disabled(*current_item)) {
    digitalWrite(current_item->led_pin, HIGH);
    current_item->end_millis = time + current_item->delay;
  } else if (is_item_completed(*current_item, time)) {
    digitalWrite(current_item->led_pin, LOW);
    reset_sequence_item(current_sequence_index_, current_item_index_);
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

void reset_sequence_item(uint8_t sequence_index, uint8_t item_index) {
  auto &item = sequences[sequence_index].items[item_index];
  item.end_millis = 0;
}

bool is_item_disabled(const sequence_item &item) {
  return item.end_millis == 0;
}
bool is_item_completed(const sequence_item &item, unsigned long time) {
  return item.end_millis < time;
}

void on_button_clicked() {
  button_clicked_ = true;
  /*
  auto current_sequence = sequences + current_sequence_index_;
  auto current_item = &current_sequence->items[current_item_index_];
  digitalWrite(current_item->led_pin, LOW);
  reset_sequence_item(current_sequence_index_, current_item_index_);

  current_sequence_index_ =
      get_next_sequence_index(current_sequence_index_, sequences_number);
  current_item_index_ = 0;
  */
}

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
