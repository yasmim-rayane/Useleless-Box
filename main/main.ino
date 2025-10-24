#include <Servo.h>

/* Function's prototypes */
void rgbFade(); // - COMPLETED
void openAnyDoor();
void openLeftDoor();
void openRightDoor();

/* ===== Constants ===== */
Servo leftDoor;
Servo rightDoor;
Servo skeletonBody;
Servo skeletonHand;

const int leftDoorPin = 7;
const int rightDoorPin = 10;
const int skeletonBodyPin = 4;
const int skeletonHandPin = 9;

// PWM pins for the RGB strip
const int redPin = 11;
const int greenPin = 5;
const int bluePin = 6;

// Other pins
const int toogleSwitchPin = 13;

/* Initial config */
void setup(){
  // Servo's attach
  leftDoor.attach(leftDoorPin);
  rightDoor.attach(rightDoorPin);
  skeletonBody.attach(skeletonBodyPin);
  skeletonHand.attach(skeletonHandPin);

  // Servo's reset
  leftDoor.write(0);
  rightDoor.write(0);
  skeletonBody.write(0);
  skeletonHand.write(0);

  // RGB strip
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // Toogle switch
  pinMode(toogleSwitchPin, INPUT);
}

/* Most important code below BE CAREFUL*/
void loop(){
  int isToogleSwitchSwitched = digitalRead(toogleSwitchPin);
  isToogleSwitchSwitched ? randomDoor() : rgbFade();
}

void randomDoor(){
  int num = random(1, 3);
  num == 1 ? openLeftDoor() : openRightDoor();
}

void openLeftDoor(){
  leftDoor.write(90);
  delay(2000);
  // someAction();
  leftDoor.write(0);
}

void openRightDoor(){
  rightDoor.write(90);
  delay(2000);
  // someAction();
  rightDoor.write(0);
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
    analogWrite(redPin, redValue);
    analogWrite(greenPin, greenValue);
    analogWrite(bluePin, blueValue);
  }
}
