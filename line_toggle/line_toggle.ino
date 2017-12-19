// Pinout
#define FRAME_CLK 31 // Currently not used
#define LINE_CLK 33
#define MOD_IN 35
#define LASER_OUT 37
bool mod;
bool line;
bool prev_line;

#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))

// Laser stim definitions
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
      // Use "SET" and "CLR" instead of digitalWrite to avoid overhead
      SET(PORTB, 2);
      delayMicroseconds(PULSE_ON_DURATION);
      CLR(PORTB, 2);
    }
  }
}
