#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;

void main(void){
	int i,j, dec_inc;                        // dec_inc = 1 when counting up in state1, and 0 when counting down
	unsigned int up_down = 0;               // For state1 - 0 means counting up, 1 means count down.
	int LED_state[] = {0,0x01};             // LED_state[0] = LED state of state 1, LED_state[1] = LED state of state 2
 	state = state0;                         // start in idle state on RESET
  	lpm_mode = mode0;                       // start in idle state on RESET
	int pwm_freq = 2;                       // Freq = 2KHz
  	sysConfig();
	volatile int Switches_State;

	while(1){
		switch(state){
			case state0:				     // Sleep Mode
				clrLEDs();
				enterLPM(lpm_mode);
			break;
				
			case state1:								// LED count up/down binary
				disable_interrupts();
				print2LEDs(LED_state[0]);  				 // Print the last state to LED
				// The state runs for 10 sec, each iteration takes 0.5 sec --> 20 iterations needed
				dec_inc = (up_down == 0) ? 1 : -1; 		// If up_down = 0 --> dec_inc = 1, else dec_inc = -1
				up_down = 1 - up_down;

				for (i = 0; i < 20; i++){
					incLEDs(dec_inc);
					// Delay:
					for(j=0; j<8; j++){					// 62.5m * 8 = 0.5[sec]
						delay(LEDs_SHOW_RATE);			// delay of 62.5 [ms]
					}
				}
				// 10 seconds passed
			LED_state[0] = getLEDs();	        		// Storing the latest LED state
				state = state0;
				enable_interrupts();
				break;
				
			case state2:
				disable_interrupts();
				print2LEDs(LED_state[1]);               // Print the last state to LED
						// 0.5 sec delay
				for(j=0; j<8; j++){		       	 		// 62.5m * 8 = 0.5[sec]
					delay(LEDs_SHOW_RATE);	        	// delay of 62.5 [ms]
					}

				// The state runs for 7 sec, each iteration takes 0.5 sec --> 14 iterations needed
				for (i = 0; i < 14; i++){
					// Display 1 if empty, otherwise shift left by 1
					(getLEDs() == 0) ? incLEDs(1) : shlLEDs(1);
					
					// 0.5 sec delay
					for(j=0; j<8; j++){					// 62.5m * 8 = 0.5[sec]
						delay(LEDs_SHOW_RATE);			// delay of 62.5 [ms]
					}
				}
				LED_state[1] = getLEDs();	        	// Storing the latest LED state
				state = state0;
				enable_interrupts();
				break;

			case state3:
				clrLEDs();
				// 4kH with DC=75% --> '1' for 0.1875 ms, '0' for 0.0625 ms.
				while (state == state3) {
					setOutputPin();
					for(i=0; i<3; i++){	
						delay(P7OUT_RATE);				// delay 0.0625 ms 3 times = delay 0.1875 ms
					}              
					resetOutputPin();
					delay(P7OUT_RATE);              	// delay 0.0625 ms
				}
				break;
						
			case state4:
				clrLEDs();
				while (state == state4) {
					pwm(pwm_freq);
				}
				break;
		}		
	}
}