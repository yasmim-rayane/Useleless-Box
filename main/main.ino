#include <Servo.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

/*
  Useless Box with Skeleton - Main Code
  [ ] - Implement non-blocking skeleton animations for door control and improve action delays
  [ ] - Add sound effects to skeleton animations
  [ ] - Optimize servo movements for smoother operation - (for)
  [ ] - Code refactoring and structuring for better readability and maintenance
  [ ] - Final testing and debugging
*/

/* Servo */
const int SERVO_LEFT_PIN = A0;
const int SERVO_RIGHT_PIN = A2;
const int SERVO_SKELETON_BODY_PIN = A1;
const int SEQRVO_SKELETON_HAND_PIN = A3;

// Servo angles
const int DOOR_OPEN_ANGLE = 90;
const int DOOR_CLOSED_ANGLE = 0;

/* RGB Strip */
const int RGB_RED_PIN = 9;
const int RGB_GREEN_PIN = 10;
const int RGB_BLUE_PIN = 11;

/* Eyes (LEDs) */
// Assumo dois LEDs para os olhos em pinos digitais livres (5 e 6).
// Se você tiver outros pinos usados, me avise para ajustar.
const int EYE_LEFT_PIN = 9;
const int EYE_RIGHT_PIN = 10;

/* Toogle Switch */
const int TOGGLE_SWITCH_PIN = 12;

/* MP3 Player */
const int MP3_RX_PIN = 12;
const int MP3_TX_PIN = 13;

/* Skeleton eyes leds */
const int SKELETON_EYES_RIGHT_PIN = 10;
const int SKELETON_EYES_LEFT_PIN = 9;

/* Servos and MP3 player */
Servo leftDoor;
Servo rightDoor;
Servo skeletonBody;
Servo skeletonHand;

SoftwareSerial mp3Serial(MP3_RX_PIN, MP3_TX_PIN);
DFRobotDFPlayerMini playerMP3;

// Flag para saber se o DFPlayer foi inicializado com sucesso
bool mp3Initialized = false;

/* Function's prototypes */
void playSound(uint8_t track);
void triggerRandomDoor();
void openLeftDoor(); // Rever essas funções para transformar em struct
void openRightDoor(); // e essa também
void soundTest();
void rgbFade();

/* Initial config */
void setup(){
  Serial.begin(9600);
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

  // Ensure RGB is on at startup (initial color: red)
  analogWrite(RGB_RED_PIN, 255);
  analogWrite(RGB_GREEN_PIN, 0);
  analogWrite(RGB_BLUE_PIN, 0);

  // Eyes pins
  pinMode(EYE_LEFT_PIN, OUTPUT);
  pinMode(EYE_RIGHT_PIN, OUTPUT);
  // Ensure eyes are off at startup
  digitalWrite(EYE_LEFT_PIN, LOW);
  digitalWrite(EYE_RIGHT_PIN, LOW);

  // Toogle switch
  // Use internal pull-up to avoid floating input. Wiring: switch connects pin to GND when pressed.
  pinMode(TOGGLE_SWITCH_PIN, INPUT_PULLUP);

  // Initialize random seed
  randomSeed(analogRead(A5));

  // Init MP3 player
  mp3Serial.begin(9600);
  Serial.println(F("Iniciando DFPlayer..."));
  if (!playerMP3.begin(mp3Serial)) {  
    Serial.println(F("Falha ao iniciar DFPlayer Mini:"));
    Serial.println(F("1. Verifique conexoes"));
    Serial.println(F("2. Verifique o cartao SD"));
    // Não travar o loop completamente - apenas reporta o erro
  } else {
    Serial.println(F("DFPlayer Mini iniciado com sucesso!"));
    playerMP3.volume(20);  // Volume de 0 a 30
    mp3Initialized = true;
  }
}

/* Most important code below BE CAREFUL*/
void loop(){
  // Read the toggle switch with debounce and edge detection.
  // Because we use INPUT_PULLUP, the pin reads HIGH when open (not pressed)
  // and LOW when pressed (connected to GND).
  const unsigned long DEBOUNCE_MS = 50;

  static int toggleCount = 0;
  static int lastReading = HIGH;        // last raw reading from pin
  static int stableState = HIGH;       // debounced stable state
  static unsigned long lastDebounceTime = 0;

  int reading = digitalRead(TOGGLE_SWITCH_PIN);

  if (reading != lastReading) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_MS) {
    // if the reading has been stable for longer than debounce delay
    if (reading != stableState) {
      stableState = reading;

      // Detect a press event: transition from HIGH -> LOW (because of pull-up)
      if (stableState == LOW) {
        toggleCount++;
        Serial.print(F("Toggle pressed, count: "));
        Serial.println(toggleCount);
        if (toggleCount >= 3) {
          triggerRandomDoor();
          toggleCount = 0;
        }
      }
    }
  }

  lastReading = reading;

  // Always run the RGB fade so LEDs remain on even during button states
  rgbFade();

}

void triggerRandomDoor(){
  int num = random(1, 3); // 1 ou 2
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
  int r = random(0, 2); // 0 ou 1 - mapearemos 2 animações para a mão
  switch (r) {
  case 0:
    // Simply move the skeleton hand to turn off the switch
    skeletonHand.write(90);
    playSound(3); // toca a faixa 3 para essa animação da mão
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
    playSound(4); // toca a faixa 4 para essa animação da mão
    delay(1000);
    skeletonHand.write(0);
    break;
  }
}

void wakeSkeleton(){ // Randomly select a wake-up animation for the skeleton with sound effects
  // Turn eyes on when waking
  eyesOn();

  int r = random(0, 2); // 0 ou 1 - duas animações de wake
  switch (r) {
    case 0:
      // Simply wake up the skeleton body
      skeletonBody.write(90);
      playSound(1); // toca a faixa 1 para essa animação de wake
      delay(1000);
      break;

    case 1:
      // Um wake mais dramático
      skeletonBody.write(60);
      delay(400);
      skeletonBody.write(100);
      playSound(2); // toca a faixa 2 para essa animação de wake
      delay(800);
      break;
  }

  handSkeleton(); // After waking up, make the skeleton hand turn off the switch
  skeletonBody.write(0); // Return skeleton to initial position

  // Turn eyes off after returning to rest
  eyesOff();
}

void soundTest() {
  // Função de teste simples para tocar a faixa 1
  if (&playerMP3) {
    playSound(1);
  }
}

void playSound(uint8_t track) {
  // track: número da faixa no cartão SD (1..N)
  if (mp3Initialized) {
    playerMP3.play(track);
  } else {
    // Se player não estiver disponível, imprimir no Serial para debug
    Serial.print(F("playSound (MP3 nao inicializado). Track: "));
    Serial.println(track);
  }
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

// Eyes control
void eyesOn() {
  // If you prefer PWM brightness, replace digitalWrite with analogWrite(…, value);
  digitalWrite(EYE_LEFT_PIN, HIGH);
  digitalWrite(EYE_RIGHT_PIN, HIGH);
}

void eyesOff() {
  digitalWrite(EYE_LEFT_PIN, LOW);
  digitalWrite(EYE_RIGHT_PIN, LOW);
}