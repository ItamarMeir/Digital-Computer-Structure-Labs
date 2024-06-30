#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;
int Key = 16;
char idiom_recorder[32];
const char keyboard[16][4] = {"1G","2H","3I","CJ","4K","5L","6M","DN","7O","8P","9Q","ER","AST","0UV","BWX","FYZ"};
char data_matrix[10][40] = {"An apple a day keeps the doctor away",
                            "climb on the bandwagon",
                            "Dot the i's and cross the t's",
                            "He who pays the piper calls the tune",
                            "The pen is mightier than the sword",
                            "The pot calling the kettle black",
                            "shed crocodile tears",
                            "Close but no cigar",
                            "Cut from the same cloth",
                            "Strike while the iron's hot"};
char strMerge[80];
char leds[9] = {128,64,32,16,8,4,23,13,40};
char* leds_ptr = leds;

void main(void){
  
  state = state0;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();
  lcd_init();
  
  while(1){
    lcd_clear();
	switch(state){
	  case state0:
                enterLPM(lpm_mode);
		break;
		 
	  case state1:
        clear_LEDs();
        idiom_rec();
        if(state == state1){
            state = state0;
        }
		break;
		 
	  case state2:
      clear_LEDs();
	    merge();
		break;       
                
	  case state3:
	    shift_leds();
      clear_LEDs();    // Clear LEDs
		break;

    case state4:
        state = state0;
        break; 
    }
  }
}
