#include  "../header/api.h"    		// private library - API layer
#include  "../header/hal.h"     // private library - HAL layer

//-------------------------------------------------------------
//            Print SWs value onto LEDs
//-------------------------------------------------------------
void printSWs2LEDs(void){
	unsigned char ch;
	ch = readSWs();
	print2LEDs(ch);
}
//--------------------------------------------------------------------
//            Print array to LEDs array with rate of LEDs_SHOW_RATE
//--------------------------------------------------------------------            
void printArr2SWs(char Arr[], int size, unsigned int rate){
	unsigned int i;
	
	for(i=0; i<size; i++){
		print2LEDs(Arr[i]);
		delay(rate);
	}
}

unsigned int clk_overflows = 0;
unsigned int t_0;
unsigned int t_1;
unsigned int count = 0;
unsigned int SMCLK_freq = 1048576;

unsigned char ext_timer_freq(){
	unsigned int frequncy = 0;
    // Begin capturing
	int i;
	for (i = 0; i < N_of_measurements; i++){			// Average over N_of_measurements
		Timer_A1_start();
		while (count != 2){
			// wait
		}
		Timer_A1_stop();
		count = 0;

		if (t_0 > t_1){										// If we got overflow
			frequncy += t_1 - t_0 + 65536;					// Add the period to the total, and add the overflow (2^16 = 65536)
		}
		else{
			frequncy += t_1 - t_0;							// Add the period to the total
		}
		
	}

	frequncy = frequncy / N_of_measurements; 			// Average the total
	frequncy = SMCLK_freq / frequncy; 					// Calculate the frequency
	return frequncy;
}

void write_freq_LCD(unsigned int freq){
	char strFreq[6] = {'\0'};
	sprintf(strFreq, "%d", freq);
	lcd_home();
	lcd_cursor_right();
	lcd_cursor_right();
	lcd_cursor_right();
	lcd_cursor_right();
	lcd_puts(strFreq);
	cursor_off;
}

void write_MM_LCD(unsigned int min){

	char strMin[2] = {'\0'};
	sprintf(strMin, "%d", min);
	lcd_home();
	lcd_puts(strMin);
	lcd_cursor_right();
	cursor_off;
}

void write_SS_LCD(unsigned int sec){
	char strSec[2] = {'\0'};
	sprintf(strSec, "%d", sec);
	lcd_home();
	lcd_cursor_right();
	lcd_cursor_right();
	lcd_cursor_right();
	lcd_puts(strSec);
	cursor_off;
}



  

