// Pinout
#define FRAME_CLK 31 // Port C, pin 6
#define LINE_CLK 33  // Port C, pin 4 
#define MOD_IN 35    // Port C, pin 2
#define LASER_OUT 37 // Port C, pin 0

bool mod;
bool frame, prev_frame;
bool line, prev_line;

// Use direct port manipulation for speed
#define READ(x,y) (x&(1<<y))
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))

// Hard-code subframe parameters. Here, we:
//  - Assume that the full frame is 512 lines
//  - Split the full frame into N=5 separate "subframes", there are N-1 "lines" where the
//      laser state will transition
#define SUBFRAME_LINE1 103
#define SUBFRAME_LINE2 205
#define SUBFRAME_LINE3 307
#define SUBFRAME_LINE4 409
#define IM_LINE_OFFSET 2

// Amount of time to delay for transition of laser at the end of frame. This
//  is useful for dual-site imaging, where the slave microscope frames lag the
//  master microscope frames by a few lines.
#define TOGGLE_DELAY 300 // us

#define S_IM1 0 // Image subfields 1, 3, 5
#define S_IM2 1 // Image subfield 2, 4
int imaging_state = S_IM1;
int line_idx = 1;

bool mod_enabled = false;

void setup() {
  pinMode(FRAME_CLK, INPUT);
  pinMode(LINE_CLK, INPUT);
  pinMode(MOD_IN, INPUT);
  pinMode(LASER_OUT, OUTPUT);

  frame = false;
  line = false;
}

void loop() {
  mod = READ(PINC,2);

  prev_frame = frame;
  frame = READ(PINC,6);
  
  prev_line = line;
  line = READ(PINC,4);
  

  // First rising edge on FRAME clk after MOD_IN is high
  if (mod && !mod_enabled && !prev_frame && frame)
  {
    mod_enabled = true;
    
    imaging_state = S_IM1;
    line_idx = 1;
  }

  if (!mod_enabled)
    CLR(PORTC,0); // Keep laser OFF
  else
  {
    if (prev_frame && !frame) // Falling edge on FRAME clk
    {
      mod_enabled = mod;

      delayMicroseconds(TOGGLE_DELAY);
      
      switch (imaging_state)
      {
        case S_IM1:
          imaging_state = S_IM2;
          if (mod_enabled)
            SET(PORTC,0); // Laser ON
          break;
        case S_IM2:
          imaging_state = S_IM1;
          CLR(PORTC,0); // Laser OFF
          break;
      }
      line_idx = 1;
    }

    if (!prev_line && line) // Rising edge on LINE clk
    {
      switch (imaging_state)
      {
        case S_IM1:
          if (line_idx < SUBFRAME_LINE1 + IM_LINE_OFFSET)
            CLR(PORTC,0); // Laser OFF
          else if (line_idx < SUBFRAME_LINE2 - IM_LINE_OFFSET)
            SET(PORTC,0); // Laser ON
          else if (line_idx < SUBFRAME_LINE3 + IM_LINE_OFFSET)
            CLR(PORTC,0); // Laser OFF
          else if (line_idx < SUBFRAME_LINE4 - IM_LINE_OFFSET)
            SET(PORTC,0); // Laser ON
          else
            CLR(PORTC,0); // Laser OFF
          break;
          
        case S_IM2:
          if (line_idx < SUBFRAME_LINE1 - IM_LINE_OFFSET)
            SET(PORTC,0); // Laser ON
          else if (line_idx < SUBFRAME_LINE2 + IM_LINE_OFFSET)
            CLR(PORTC,0); // Laser OFF
          else if (line_idx < SUBFRAME_LINE3 - IM_LINE_OFFSET)
            SET(PORTC,0); // Laser ON
          else if (line_idx < SUBFRAME_LINE4 + IM_LINE_OFFSET)
            CLR(PORTC,0); // Laser OFF
          else
            SET(PORTC,0); // Laser ON
          break;
      }

      line_idx++;
    }
  }

}
