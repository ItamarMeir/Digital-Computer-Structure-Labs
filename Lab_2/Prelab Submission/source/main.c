#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;

void main(void){
	unsigned int currernt_freq = 0;		// Current frequency for state1
	unsigned int prev_freq = 0;			// Previous frequency for state1

  	unsigned int min = 0;   			// Previous minuites for state2
	unsigned int sec = 0;   			// Previous seconds for state2	
	sysConfig();
	enable_interrupts();  // Enable to cut the states

	while(1){
		switch(state){
			case state0:				     // Sleep Mode
				lcd_clear();
				enterLPM(lpm_mode);
			break;
				
			case state1: 	// PWM input from P2.4 massuring the frequency with Timer_A1. Input capture mode, SMCLK								
				lcd_clear();
				write_freq_tmp_LCD();		// Write template of Frequency
				write_freq_LCD(0);
				while (state == state1){
					currernt_freq = ext_timer_freq();	// Get the current frequency
					if (currernt_freq != prev_freq){	// If the frequency has changed
						prev_freq = currernt_freq;		// Update the previous frequency						
						write_freq_LCD(currernt_freq);				// Write the frequency to the LCD
					}
				}
				break;
				
			case state2:
				lcd_clear();
				write_time_tmp_LCD();		// Write template of Time
				write_MM_LCD(min);			// Write the minutes to the LCD
				write_SS_LCD(sec);			// Write the seconds to the LCD
				while (state == state2){
					while (read_single_SW(SW0) == 1){ // While the switch is pressed
						Timer0_A0_start();		// Start the timer and interrupt
					}
				}
				Timer0_A0_stop();
				break;

			case state3:
				

				break;
						
			case state4:
				

				break;
		}		
	}
}