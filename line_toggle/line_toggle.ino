// Pinout
#define FRAME_CLK 31 // Port C, pin 6; Currently not used
#define LINE_CLK 33  // Port C, pin 4 
#define MOD_IN 35    // Port C, pin 2
#define LASER_OUT 37 // Port C, pin 0
bool mod;
bool line;
bool prev_line;

// Use direct port manipulation for speed
#define READ(x,y) (x&(1<<y))
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
  mod = READ(PINC,2);

  if (!mod)
  {
    CLR(PORTC,0); // Keep laser off
  }
  else
  {
    prev_line = line;
    line = READ(PINC,4);

    if (prev_line && !line) // Falling edge
    {
      SET(PORTC,0);
      delayMicroseconds(PULSE_ON_DURATION);
      CLR(PORTC,0);
    }
  }
}
