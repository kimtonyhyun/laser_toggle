// Pinout
#define FRAME_CLK 31 // Port C, pin 6
#define LINE_CLK 33  // Port C, pin 4 
#define MOD_IN 35    // Port C, pin 2
#define LASER_OUT 37 // Port C, pin 0

// Use direct port manipulation for speed
#define READ(x,y) (x&(1<<y))
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))

// Hard-code subframe parameters. Here, we:
//  - Assume that the full frame is 512 lines
//  - Split the full frame into 3 separate "subframes"
#define NUM_SUBFRAMES 3
#define SUBFRAME1_LINE_START 1
#define SUBFRAME1_LINE_END 170
#define SUBFRAME2_LINE_START 171
#define SUBFRAME2_LINE_END 341
#define SUBFRAME3_LINE_START 342
#define SUBFRAME3_LINE_END 512

volatile int subframe_state = 1;
volatile int laser_off_line;
volatile int laser_on_line;
volatile int line_idx;

volatile bool mod_enabled = false;

void update_mod() // Called on RISING edge of FRAME clk
{
  if (~mod_enabled && READ(PINC,2))
  {
    mod_enabled = true;

    subframe_state = 1;
    laser_off_line = SUBFRAME1_LINE_START;
    laser_on_line = SUBFRAME1_LINE_END;
    line_idx = 1;
  }
}

void update_lines() // Called on FALLING edge of FRAME clk
{
  if (mod_enabled)
  {
    switch (subframe_state) // Update laser timing for the next frame
    {
      case 1:
        subframe_state = 2;
        laser_off_line = SUBFRAME2_LINE_START;
        laser_on_line = SUBFRAME2_LINE_END;
        break;
        
      case 2:
        subframe_state = 3;
        laser_off_line = SUBFRAME3_LINE_START;
        laser_on_line = SUBFRAME3_LINE_END;
        break;
        
      case 3:
        subframe_state = 1;
        laser_off_line = SUBFRAME1_LINE_START;
        laser_on_line = SUBFRAME1_LINE_END;
        break;
    }
    line_idx = 1;
  }
}

void set_laser() // Called on RISING edge of LINE clk
{
  if (mod_enabled)
  {
    if (line_idx < laser_off_line)
      SET(PORTC,0); // Laser ON
    else if (line_idx < laser_on_line)
      CLR(PORTC,0); // Laser OFF
    else
      SET(PORTC,0); // Laser ON
  
    line_idx++;
  }
}

void setup() {
  pinMode(FRAME_CLK, INPUT);
  pinMode(LINE_CLK, INPUT);
  pinMode(MOD_IN, INPUT);
  pinMode(LASER_OUT, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(FRAME_CLK), update_mod, RISING);
  attachInterrupt(digitalPinToInterrupt(FRAME_CLK), update_lines, FALLING);
  attachInterrupt(digitalPinToInterrupt(LINE_CLK), set_laser, RISING);
}

void loop() {
  if (!READ(PINC,2)) // MOD_IN
  {
    CLR(PORTC,0); // Keep laser off
    mod_enabled = false;
  }
}
