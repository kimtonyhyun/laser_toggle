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
#define NUM_SUBFRAMES 3
#define SUBFRAME1_LINE_START 1
#define SUBFRAME1_LINE_END 170
#define SUBFRAME2_LINE_START 171
#define SUBFRAME2_LINE_END 341
#define SUBFRAME3_LINE_START 342
#define SUBFRAME3_LINE_END 512

int subframe_state = 1;
int laser_off_line = SUBFRAME1_LINE_START;
int laser_on_line = SUBFRAME1_LINE_END;
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
      
      subframe_state = 1;
      laser_off_line = SUBFRAME1_LINE_START;
      laser_on_line = SUBFRAME1_LINE_END;
      line_idx = 1;
    }

    if (mod_enabled)
    {
      // Falling edge on FRAME clk -- update opto timing for next frame
      if (prev_frame && !frame)
      {
        switch (subframe_state)
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

      if (!prev_line && line) // Rising edge on LINE clk
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
  }
}
