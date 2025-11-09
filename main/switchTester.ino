const int switchPin = 8;

void setup() {
  Serial.begin(9600);
  pinMode(switchPin, INPUT);
  Serial.println(F("switchTester iniciado. Pressione o interruptor para testar."));
}

void loop() {
  if (digitalRead(switchPin) == LOW) {
    Serial.println(F("Interruptor pressionado!"));
    delay(1000); // Debounce delay
  }
}
