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
//  - Split the full frame into 3 separate "subframes"
#define SUBFRAME_LINE1 171
#define SUBFRAME_LINE2 342

int imaging_state = 0; // 0 <==> Image subfields 1 and 3; 1 <==> Image subfield 2
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
  
  if (!mod)
  {
    CLR(PORTC,0); // Keep laser off
    mod_enabled = false;
  }
  else
  {
    // First rising edge on FRAME clk after MOD_IN is high
    if (!mod_enabled && !prev_frame && frame)
    {
      mod_enabled = true;
      
      laser_state = 0;
      line_idx = 1;
    }

    if (mod_enabled)
    {
      if (prev_frame && !frame) // Falling edge on FRAME clk
      {
        switch (imaging_state)
        {
          case 0:
            imaging_state = 1;
            break;
          case 1:
            imaging_state = 0;
            break;
        }
      }

      if (!prev_line && line) // Rising edge on LINE clk
      {
        switch (imaging_state)
        {
          case 0: // Image subfields 1 and 3
            if (line_idx < SUBFRAME_LINE1)
              CLR(PORTC,0); // Laser OFF
            else if (line_idx < SUBFRAME_LINE2)
              SET(PORTC,0); // Laser ON
            else
              CLR(PORTC,0); // Laser OFF
            break;
            
          case 1: // Image subfield 2
            if (line_idx < SUBFRAME_LINE1)
              SET(PORTC,0); // Laser ON
            else if (line_idx < SUBFRAME_LINE2)
              CLR(PORTC,0); // Laser OFF
            else
              SET(PORTC,0); // Laser ON
            break;
        }

        line_idx++;
      }
    }
  }
}
