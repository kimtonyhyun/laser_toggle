// Pinout
#define FRAME1_IN 45
#define FRAME2_IN 47
#define MOD_IN 49 // "Modulate"
#define LASER_OUT 51
bool mod;
bool frame;
bool prev_frame;

// FSM definitions
#define S_IDLE 0
#define S_PULSE_HIGH 1
int state;

// Laser stim definitions
#define PULSE_DELAY 500 // us
#define PULSE_ON_DURATION 5 // ms

void setup() {
  pinMode(FRAME1_IN, INPUT);
  pinMode(FRAME2_IN, INPUT);
  pinMode(MOD_IN, INPUT);
  pinMode(LASER_OUT, OUTPUT);
  state = S_IDLE;
}

void loop() {
  mod = digitalRead(MOD_IN);

  if (!mod)
  {
    digitalWrite(LASER_OUT, 0); // Keep laser off
    state = S_IDLE;
  }
  else
  {
    prev_frame = frame;
    frame = digitalRead(FRAME2_IN); // Select FRAME1 or FRAME2
    
    switch (state)
    { 
      case S_IDLE:
        digitalWrite(LASER_OUT, 0);
        if (prev_frame && !frame) // Falling edge      
          state = S_PULSE_HIGH;
        break;
  
      case S_PULSE_HIGH:
        delayMicroseconds(PULSE_DELAY);
        digitalWrite(LASER_OUT, 1);
        delay(PULSE_ON_DURATION);
        state = S_IDLE;
        break;
        
      default:
        state = S_IDLE;
    }
  }
}
