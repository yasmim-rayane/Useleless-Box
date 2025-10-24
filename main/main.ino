#include <Servo.h>

/*
  Useless Box with Skeleton - Main Code
  [ ] - Implement non-blocking skeleton animations for door control and improve action delays
  [ ] - Add sound effects to skeleton animations
  [ ] - Optimize servo movements for smoother operation - (for)
  [ ] - Code refactoring and structuring for better readability and maintenance
  [ ] - Final testing and debugging
*/

/* Constants */
const int SERVO_LEFT_PIN = 7;
const int SERVO_RIGHT_PIN = 10;
const int SERVO_SKELETON_BODY_PIN = 4;
const int SERVO_SKELETON_HAND_PIN = 9;

const int RGB_RED_PIN = 11;
const int RGB_GREEN_PIN = 5;
const int RGB_BLUE_PIN = 6;

const int TOGGLE_SWITCH_PIN = 13;

// Servo angles
const int DOOR_OPEN_ANGLE = 45;
const int DOOR_CLOSED_ANGLE = 0;

/* Servos */
Servo leftDoor;
Servo rightDoor;
Servo skeletonBody;
Servo skeletonHand;

/* Function's prototypes */
void triggerRandomDoor();
void openLeftDoor(); // Rever essas funções para transformar em struct
void openRightDoor(); // e essa
void rgbFade();

/* Initial config */
void setup(){
  // Servo's attach
  leftDoor.attach(SERVO_LEFT_PIN);
  rightDoor.attach(SERVO_RIGHT_PIN);
  skeletonBody.attach(SERVO_SKELETON_BODY_PIN);
  skeletonHand.attach(SERVO_SKELETON_HAND_PIN);

  // Servo's reset
  leftDoor.write(0);
  rightDoor.write(0);
  skeletonBody.write(0);
  skeletonHand.write(0);

  // RGB strip
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  // Toogle switch
  pinMode(TOGGLE_SWITCH_PIN, INPUT);
}

/* Most important code below BE CAREFUL*/
void loop(){
  int isToogleSwitchSwitched = digitalRead(TOGGLE_SWITCH_PIN);
  isToogleSwitchSwitched ? triggerRandomDoor() : rgbFade();
}

void triggerRandomDoor(){
  int num = random(1, 3);
  num == 1 ? openLeftDoor() : openRightDoor();
}

void openLeftDoor(){
  leftDoor.write(DOOR_OPEN_ANGLE);
  delay(500);
  wakeSkeleton();
  delay(500);
  leftDoor.write(DOOR_CLOSED_ANGLE);
}

void openRightDoor(){
  rightDoor.write(DOOR_OPEN_ANGLE);
  delay(500);
  handSkeleton();
  delay(500);
  rightDoor.write(DOOR_CLOSED_ANGLE);
}

void handSkeleton(){ // Randomly select a hand animation for the skeleton to turn off the switch
  switch (0) {// random(0, 10)
  case 0:
    // Simply move the skeleton hand to turn off the switch
    skeletonHand.write(90);
    delay(1000);
    skeletonHand.write(0);
    break;
  
  case 1:
    // Move the skeleton hand to wave
    skeletonHand.write(45);
    delay(1000);
    skeletonHand.write(30);
    delay(1000);
    skeletonHand.write(90);
    delay(1000);
    skeletonHand.write(0);
    break;

  case 2:
    
    break;

  case 3:
    break;

  case 4:
    break;

  case 5:
    break;

  case 6:
    break;

  case 7:
    break;

  case 8:
    break;

  case 9:
    break;

  default:
    break;
  }
}

void wakeSkeleton(){ // Randomly select a wake-up animation for the skeleton with sound effects
  switch (0) { // random(0, 10)
    case 0:
      // Simply wake up the skeleton body
      skeletonBody.write(90);
      delay(1000);
      break;
    
    case 1:
      break;

    case 2:
      break;

    case 3:
      break;

    case 4:
      break;

    case 5:
      break;

    case 6:
      break;

    case 7:
      break;

    case 8:
      break;

    case 9:
      break;

    default:
      break;
    }
  handSkeleton(); // After waking up, make the skeleton hand turn off the switch
  skeletonBody.write(0); // Return skeleton to initial position
}

void rgbFade() {
  // Local static variables
  static int redValue = 255;
  static int greenValue = 0;
  static int blueValue = 0;

  static int fadeStep = 5;
  static unsigned long previousMillis = 0;
  static const unsigned long interval = 70;
  static int colorPhase = 0; // 0 = R→G, 1 = G→B, 2 = B→R

  unsigned long currentMillis = millis();

  // Run this block only when enough time has passed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Handle color transitions
    switch (colorPhase) {
      case 0: // Red → Green
        redValue -= fadeStep;
        greenValue += fadeStep;
        if (redValue <= 0 && greenValue >= 255) {
          redValue = 0;
          greenValue = 255;
          colorPhase = 1;
        }
        break;

      case 1: // Green → Blue
        greenValue -= fadeStep;
        blueValue += fadeStep;
        if (greenValue <= 0 && blueValue >= 255) {
          greenValue = 0;
          blueValue = 255;
          colorPhase = 2;
        }
        break;

      case 2: // Blue → Red
        blueValue -= fadeStep;
        redValue += fadeStep;
        if (blueValue <= 0 && redValue >= 255) {
          blueValue = 0;
          redValue = 255;
          colorPhase = 0;
        }
        break;
    }

    // Update PWM outputs
    analogWrite(RGB_RED_PIN, redValue);
    analogWrite(RGB_GREEN_PIN, greenValue);
    analogWrite(RGB_BLUE_PIN, blueValue);
  }
}