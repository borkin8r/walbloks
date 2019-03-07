//imports the SPI library (needed to communicate with Gamebuino's screen)
#include <SPI.h>
//imports the Gamebuino library
#include <Gamebuino.h>
//creates a Gamebuino object named gb
Gamebuino gb;

//nokia 5110 84x48
int dropper_x = LCDWIDTH/2;
int dropper_Y = 2;
int dropper_vx = 1;
int drop_vy = 1;

// the setup routine runs once when Gamebuino starts up
void setup(){
  // initialize the Gamebuino object
  gb.begin();
  //display the main menu:
  gb.titleScreen(F("Walbloks"));
  gb.popup(F("Let's go!"), 100);
}

// the loop routine runs over and over again forever
void loop(){
  //updates the gamebuino (the display, the sound, the auto backlight... everything)
  //returns true when it's time to render a new frame (20 times/second)
  if(gb.update()){
    //prints Hello World! on the screen
    gb.display.println(F("Score: "));

    dropper_x += dropper_vx;
    
    if(dropper_x < 0 || dropper_x > LCDWIDTH) {
      dropper_vx = -dropper_x;
      gb.sound.playTick();
    }
  }
}
