#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;
unsigned int Meas1, Meas2;

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
                freqMeas();
		break;
		 
	  case state2:
		CountUp();
		break;       
                
	  case state3:
		tone_generator();
		break;
                
          case state4:
		break;  
        }
  }
}