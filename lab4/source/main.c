#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;

unsigned int num = 0;
unsigned int X = 0xFFFF;
unsigned int X_flag = 0;
char timerInput[6];

void main(void){
  
  state = state8;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();
  lcd_init();
  
  while(1){
    lcd_clear();
	switch(state){
	  case state8:
	      enable_UARTRX_interrupts();          // Enable USCI_A0 RX interrupt
	      enterLPM(lpm_mode);                   // enter low power mode
		  break;
		 
	  case state1:
	      enable_UARTRX_interrupts();           // Enable USCI_A0 RX interrupt
	      blinkRGB();                            // blink RGB LED
		  break;
		 
	  case state2:
	      enable_UARTRX_interrupts();     
	      count();                                 // count up to LCD
		  break;
                
	  case state3:
	      enable_UARTRX_interrupts();     
	      buzzer();                                   // shift buzzer frequency
	      break;
            
	  case state4:                                        // Set X
        if(state == state4){
            state = state8;
        }
        set_X();
		break;   

      case state5:                              // Measure POT
          enable_UARTRX_interrupts();     
          measPOT();
          break;

      case state6:                          // Reset num value and clear LCD
          num = 0;
          state = state8;
          lcd_clear();
          break;

      case state7:                            // Menu display on PC side
          enable_UARTRX_interrupts();     
          state = state8;
          break;

	}
  }
}
