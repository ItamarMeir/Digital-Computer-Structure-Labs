#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;

void main(void){
  	
	sysConfig();
	

	while(1){
		switch(state){
			case state0:				     // Sleep Mode
				lcd_clear();
				enterLPM(lpm_mode);
			break;
				
			case state1:								// PWM input from P2.4 massuring the frequency with Timer_A1. Input capture mode, SMCLK
				unsigned int clk_overflows = 0;

				break;
				
			case state2:
				

				break;

			case state3:
				

				break;
						
			case state4:
				

				break;
		}		
	}
}