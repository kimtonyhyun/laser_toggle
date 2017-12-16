// Pinout
#define USER_INPUT 52
#define OUTPUT_TO_LASER 53

void setup() {
  pinMode(USER_INPUT, INPUT);
  pinMode(OUTPUT_TO_LASER, OUTPUT);
}

void loop() {
  digitalWrite(OUTPUT_TO_LASER, digitalRead(USER_INPUT));
}
