// Tony Hyun Kim
// kimth@stanford.edu
// Description: Send out digital pulses of defined width at regular intervals

// Pinout
#define USER_INPUT 52
#define OUTPUT_TO_LASER 53

// FSM definitions
#define S_IDLE 0
#define S_PULSE_HIGH 1
#define S_PULSE_LOW 2
int state;

// Laser stim definitions
#define PULSE_RATE 20 // Hz
#define PULSE_ON_DURATION 5 // ms
#define PULSE_OFF_DURATION (1000/PULSE_RATE - PULSE_ON_DURATION)

void setup() {
  // put your setup code here, to run once:
  pinMode(USER_INPUT, INPUT);
  pinMode(OUTPUT_TO_LASER, OUTPUT);
  state = S_IDLE;
}

void loop() {
  bool in = digitalRead(USER_INPUT);

  in = true; // constantly toggle
  switch (state) { 
    case S_IDLE:
      digitalWrite(OUTPUT_TO_LASER, 0); // Keep laser off
      if (in)
        state = S_PULSE_HIGH;
      break;

    case S_PULSE_HIGH:
      digitalWrite(OUTPUT_TO_LASER, 1);
      delay(PULSE_ON_DURATION);
      state = S_PULSE_LOW;
      break;

    case S_PULSE_LOW:
      digitalWrite(OUTPUT_TO_LASER, 0);
      delay(PULSE_OFF_DURATION);
      if (in)
        state = S_PULSE_HIGH;
      else
        state = S_IDLE;
      break;

    default:
      state = S_IDLE;
  }
}
