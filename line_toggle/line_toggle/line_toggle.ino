// Pinout
#define FRAME_CLK 45 // Currently not used
#define LINE_CLK 47
#define MOD_IN 49
#define LASER_OUT 51
bool mod;
bool line;
bool prev_line;

// Laser stim definitions
#define PULSE_DELAY 2 // us
#define PULSE_ON_DURATION 10 // us

void setup() {
  pinMode(FRAME_CLK, INPUT);
  pinMode(LINE_CLK, INPUT);
  pinMode(MOD_IN, INPUT);
  pinMode(LASER_OUT, OUTPUT);
}

void loop() {
  mod = digitalRead(MOD_IN);

  if (!mod)
  {
    digitalWrite(LASER_OUT, 0); // Keep laser off
  }
  else
  {
    prev_line = line;
    line = digitalRead(LINE_CLK);

    if (prev_line && !line) // Falling edge
    {
      delayMicroseconds(PULSE_DELAY);
      digitalWrite(LASER_OUT, 1);
      delayMicroseconds(PULSE_ON_DURATION);
      digitalWrite(LASER_OUT, 0);
    }
  }
}
