//imports the SPI library (needed to communicate with Gamebuino's screen)
#include <SPI.h>
//imports the Gamebuino library
#include <Gamebuino.h>
//creates a Gamebuino object named gb
Gamebuino gb;

//nokia 5110 84x48

const int DROPPER_SIZE = 8;
const int FPS = 20;
const int MAX_NUM_BLOCKS = (LCDHEIGHT / DROPPER_SIZE) * (LCDWIDTH / DROPPER_SIZE);
int dropper_x = LCDWIDTH / 2;
int dropper_y = LCDHEIGHT - (DROPPER_SIZE * 2);
int dropper_vx = 1;
int drop_vy = 1;
int dropped_block_count = 0;
int score = 100;
int count = 0;
int dropped_heighest_point = LCDHEIGHT;
struct DroppedBlocks {
  int x[MAX_NUM_BLOCKS];
  int y[MAX_NUM_BLOCKS];
  int vy[MAX_NUM_BLOCKS];
} dropped_blocks;

// the setup routine runs once when Gamebuino starts up
void setup(){
  // initialize the Gamebuino object
  gb.begin();
  //display the main menu:
  gb.titleScreen(F("Walbloks"));
  //gb.popup(F("Let's go!"), 100);
}

// the loop routine runs over and over again forever
void loop(){
  //updates the gamebuino (the display, the sound, the auto backlight... everything)
  //returns true when it's time to render a new frame (20 times/second)
  if(gb.update()){

    //check for gameover
    if(dropper_y > DROPPER_SIZE / 2) {
      count++;

      //////////////////////////////////////////
      //update dropper
      dropper_x += dropper_vx;
      
      if(dropper_x < 0 || dropper_x > LCDWIDTH - DROPPER_SIZE) {
        dropper_vx = -dropper_vx;      
        gb.sound.playTick();
      }

      //////////////////////////////////////////
      //update droppedblocks
      for(int i=0; i < dropped_block_count; i++) {
        if (dropped_blocks.vy[i] != 0) { //filter offscreen and frozen blocks
          dropped_blocks.y[i] += drop_vy;
          if (dropped_blocks.y[i] >= LCDHEIGHT - DROPPER_SIZE) { //stop from falling through bottom
            dropped_blocks.vy[i] = 0;
            dropped_blocks.y[i] = LCDHEIGHT - DROPPER_SIZE;

            if(dropped_heighest_point > LCDHEIGHT - DROPPER_SIZE) {
              dropped_heighest_point = LCDHEIGHT - DROPPER_SIZE;
            }
          }
          else { //check for frozen block collision
            for(int j = 0; j < dropped_block_count; j++) {
              if(i != j && dropped_blocks.vy[j] == 0) { //don't compare to itself and is comparing to a frozen block
                //check horizontal and vertical overlap
                if(gb.collideRectRect(dropped_blocks.x[i], dropped_blocks.y[i], DROPPER_SIZE, DROPPER_SIZE,
                                      dropped_blocks.x[j], dropped_blocks.y[j], DROPPER_SIZE, DROPPER_SIZE)) {
                  dropped_blocks.vy[i] = 0;
                  int adjusted_frozen_height = dropped_blocks.y[i] - (dropped_blocks.y[i] % 4); //what if completely overlapped
                  if(dropped_heighest_point > adjusted_frozen_height) {
                    dropped_heighest_point = adjusted_frozen_height;
                  }
                  dropped_blocks.y[i] = adjusted_frozen_height;
                }
              }
            }
          }
        }
      }
      
      //move dropper up if there is no space between heighest point and dropper
      if(dropper_y + DROPPER_SIZE >= dropped_heighest_point) {
          dropper_y -= DROPPER_SIZE;
      }

      ////////////////////////////////////
      //handle input
      if((gb.buttons.pressed(BTN_A) || gb.buttons.pressed(BTN_B)) && canDrop() && dropper_y > DROPPER_SIZE / 2) {
        dropped_blocks.x[dropped_block_count] = dropper_x;
        dropped_blocks.y[dropped_block_count] = dropper_y + DROPPER_SIZE;
        dropped_blocks.vy[dropped_block_count] = 1;
        dropped_block_count++;
        gb.sound.playTick();
      }

    }
    else {
      dropper_y = -DROPPER_SIZE;

      if(gb.buttons.pressed(BTN_C)) {
        resetGame();
      }
    }


    //prints Hello World! on the screen
    //gb.display.println(F("Score: %d"));
    gb.display.fontSize = 1;
    gb.display.cursorX = 0;
    gb.display.cursorY = 0;
    gb.display.print(dropped_block_count + (score - (count / FPS)));
    gb.display.fillRect(0, 6, LCDWIDTH, 1);

    //////////////////////////////////////
    //draw dropper
    gb.display.fillRect(dropper_x, dropper_y, DROPPER_SIZE, DROPPER_SIZE);

    /////////////////////////////////////
    //draw dropped blocks
    for(int i=0; i < dropped_block_count; i++) {
      gb.display.fillRect(dropped_blocks.x[i], dropped_blocks.y[i], DROPPER_SIZE, DROPPER_SIZE);
    }
  }
}

bool canDrop() {
  if(dropped_block_count + 1 >= MAX_NUM_BLOCKS) {
    return false;
  }
  for(int i=0; i < dropped_block_count; i++) {
    if(dropped_blocks.vy[i] != 0) {
      return false;
    }
  }
  return true;
}


void resetGame() {
  for(int i=0; i < MAX_NUM_BLOCKS; i++) {
    dropped_blocks.x[i] = -1;
    dropped_blocks.y[i] = -1;
    dropped_blocks.vy[i] = 0;
  }
  dropper_x = LCDWIDTH / 2;
  dropper_y = LCDHEIGHT - (DROPPER_SIZE * 2);
  dropped_block_count = 0;
  dropped_heighest_point = LCDHEIGHT;
  count = 0;
  score = 100;
}
