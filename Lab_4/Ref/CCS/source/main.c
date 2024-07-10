#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;

unsigned int num = 0;
unsigned int X = 0xFFFF;
unsigned int X_flag = 0;
char timerInput[6];

void main(void){
  
  state = state0;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();
  lcd_init();
  
  while(1){
    lcd_clear();
	switch(state){
	  case state0:
	      IE2 |= UCA0RXIE;
	      enterLPM(lpm_mode);
		  break;
		 
	  case state1:
	      IE2 |= UCA0RXIE;
	      blinkRGB();
		  break;
		 
	  case state2:
	      IE2 |= UCA0RXIE;
	      count();
		  break;
                
	  case state3:
	      IE2 |= UCA0RXIE;
	      buzzer();
	      break;
            
	  case state4:
        if(state == state4){
            state = state0;
        }
        set_X();
		break;   

      case state5:
          IE2 |= UCA0RXIE;
          measLDR();
          break;

      case state6:
          num = 0;
          state = state0;
          break;

      case state7:
          IE2 |= UCA0RXIE;
          enterLPM(lpm_mode);
          break;

	}
  }
}
