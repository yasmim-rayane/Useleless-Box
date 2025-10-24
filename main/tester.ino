/*
  improved.ino
  Refactored, non-blocking version with semantic improvements.
  This version DOES NOT use INPUT_PULLUP — it assumes an external pull-down resistor
  and that the button connects the pin to VCC when pressed (active = HIGH).
*/

#include <Servo.h>

/* ===== Constants ===== */
constexpr uint8_t SERVO_LEFT_PIN = 7;
constexpr uint8_t SERVO_RIGHT_PIN = 10;
constexpr uint8_t SERVO_SKELETON_BODY_PIN = 4;
constexpr uint8_t SERVO_SKELETON_HAND_PIN = 9;

constexpr uint8_t RGB_RED_PIN = 11;
constexpr uint8_t RGB_GREEN_PIN = 5;
constexpr uint8_t RGB_BLUE_PIN = 6;

constexpr uint8_t TOGGLE_SWITCH_PIN = 13; // Using INPUT; requires external pull-down resistor

// Servo angles
constexpr uint8_t DOOR_OPEN_ANGLE = 90;
constexpr uint8_t DOOR_CLOSED_ANGLE = 0;

// Timing
constexpr unsigned long DOOR_OPEN_DURATION_MS = 2000UL;
constexpr unsigned long TOGGLE_DEBOUNCE_MS = 50UL;

// RGB fade
constexpr int RGB_FADE_STEP = 5;
constexpr unsigned long RGB_INTERVAL_MS = 70UL;

/* ===== Types and state ===== */
enum class ColorPhase : uint8_t { RtoG = 0, GtoB = 1, BtoR = 2 };

struct Door {
  Servo* servo = nullptr;
  uint8_t pin = 0;
  bool isOpen = false;
  unsigned long openTimestamp = 0;
  uint8_t openAngle = DOOR_OPEN_ANGLE;
  uint8_t closedAngle = DOOR_CLOSED_ANGLE;
  unsigned long openDuration = DOOR_OPEN_DURATION_MS;

  // Attach servo and set it to closed position
  void attach(Servo& s, uint8_t p) {
    servo = &s;
    pin = p;
    servo->attach(pin);
    servo->write(closedAngle);
    isOpen = false;
  }

  // Trigger door open (non-blocking)
  void trigger() {
    if (!servo) return;
    servo->write(openAngle);
    isOpen = true;
    openTimestamp = millis();
  }

  // Update door state and close after duration
  void update() {
    if (!isOpen) return;
    if (millis() - openTimestamp >= openDuration) {
      servo->write(closedAngle);
      isOpen = false;
    }
  }
};

/* Servos and doors */
Servo leftDoorServo;
Servo rightDoorServo;
Servo skeletonBodyServo;
Servo skeletonHandServo;

Door leftDoor;
Door rightDoor;

/* RGB state (persistent across calls) */
static int redValue = 255;
static int greenValue = 0;
static int blueValue = 0;
static ColorPhase colorPhase = ColorPhase::RtoG;
static unsigned long previousRgbMillis = 0;

/* Toggle / debounce state
   Assuming wiring with external pull-down: default LOW, pressed = HIGH */
static bool lastRawToggleState = LOW;
static bool lastStableToggleState = LOW;
static unsigned long lastToggleChangeMillis = 0;

/* ===== Prototypes ===== */
bool readDebouncedToggle();
void triggerRandomDoor();
void updateDoors();
void rgbFade(); // non-blocking

/* ===== Setup ===== */
void setup() {
  // Initialize servos and doors
  leftDoor.attach(leftDoorServo, SERVO_LEFT_PIN);
  rightDoor.attach(rightDoorServo, SERVO_RIGHT_PIN);

  skeletonBodyServo.attach(SERVO_SKELETON_BODY_PIN);
  skeletonHandServo.attach(SERVO_SKELETON_HAND_PIN);

  skeletonBodyServo.write(0);
  skeletonHandServo.write(0);

  // RGB pins
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  // Toggle without internal pull-up — REQUIRES external pull-down resistor
  pinMode(TOGGLE_SWITCH_PIN, INPUT);

  // Initialize RGB outputs
  analogWrite(RGB_RED_PIN, redValue);
  analogWrite(RGB_GREEN_PIN, greenValue);
  analogWrite(RGB_BLUE_PIN, blueValue);

  // Seed random with an analog read (floating if nothing connected)
  randomSeed(analogRead(A0));
}

/* ===== Main loop ===== */
void loop() {
  // Read debounced toggle and act on rising edge (LOW -> HIGH)
  if (readDebouncedToggle() == HIGH) { // button pressed (with external pull-down)
    if (lastStableToggleState == LOW) {
      triggerRandomDoor();
    }
    lastStableToggleState = HIGH;
  } else {
    lastStableToggleState = LOW;
  }

  // Update doors (non-blocking)
  updateDoors();

  // RGB animation (non-blocking)
  rgbFade();
}

/* ===== Implementations ===== */

// Software debounce: returns stable state when debounced
bool readDebouncedToggle() {
  bool rawState = digitalRead(TOGGLE_SWITCH_PIN);
  unsigned long now = millis();

  if (rawState != lastRawToggleState) {
    // state changed; restart debounce timer
    lastToggleChangeMillis = now;
    lastRawToggleState = rawState;
  }

  // If the state has been stable for debounce interval, accept it
  if (now - lastToggleChangeMillis >= TOGGLE_DEBOUNCE_MS) {
    return lastRawToggleState;
  }

  // While debouncing, return the last stable state
  return lastStableToggleState;
}

// Trigger either left or right door at random
void triggerRandomDoor() {
  int idx = random(0, 2); // 0 or 1
  if (idx == 0) leftDoor.trigger();
  else rightDoor.trigger();
}

// Update both doors
void updateDoors() {
  leftDoor.update();
  rightDoor.update();
}

// RGB fade state machine — non-blocking
void rgbFade() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousRgbMillis < RGB_INTERVAL_MS) return;
  previousRgbMillis = currentMillis;

  switch (colorPhase) {
    case ColorPhase::RtoG:
      redValue -= RGB_FADE_STEP;
      greenValue += RGB_FADE_STEP;
      if (redValue <= 0 && greenValue >= 255) {
        redValue = 0;
        greenValue = 255;
        colorPhase = ColorPhase::GtoB;
      }
      break;

    case ColorPhase::GtoB:
      greenValue -= RGB_FADE_STEP;
      blueValue += RGB_FADE_STEP;
      if (greenValue <= 0 && blueValue >= 255) {
        greenValue = 0;
        blueValue = 255;
        colorPhase = ColorPhase::BtoR;
      }
      break;

    case ColorPhase::BtoR:
      blueValue -= RGB_FADE_STEP;
      redValue += RGB_FADE_STEP;
      if (blueValue <= 0 && redValue >= 255) {
        blueValue = 0;
        redValue = 255;
        colorPhase = ColorPhase::RtoG;
      }
      break;
  }

  // Clamp values and write PWM
  redValue = constrain(redValue, 0, 255);
  greenValue = constrain(greenValue, 0, 255);
  blueValue = constrain(blueValue, 0, 255);

  analogWrite(RGB_RED_PIN, redValue);
  analogWrite(RGB_GREEN_PIN, greenValue);
  analogWrite(RGB_BLUE_PIN, blueValue);
}