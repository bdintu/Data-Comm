#define PIN 13

void setup() {
  Serial.begin(9600);
  pinMode(PIN, OUTPUT);
}

void loop() {
  if (Serial.available() != 0) {
    digitalWrite(PIN, HIGH);
    delay(250);
    digitalWrite(PIN, LOW);
  }
}
