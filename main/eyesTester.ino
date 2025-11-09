const int eye1 = 9, eye2 = 10;

void eyesOn(){
  digitalWrite(eye1, HIGH);
  digitalWrite(eye2, HIGH);
}

void eyesOff(){
  digitalWrite(eye1, LOW);
  digitalWrite(eye2, LOW);
}

void setup() {
  Serial.begin(9600);
  pinMode(eye1, OUTPUT);
  pinMode(eye2, OUTPUT);

  Serial.println(F("eyesTester iniciado. Piscar a cada segundo."));
}

void loop() {
  eyesOn();
  Serial.println(F("Olhos ON"));
  delay(1000);
  eyesOff();
  Serial.println(F("Olhos OFF"));
  delay(1000);
}