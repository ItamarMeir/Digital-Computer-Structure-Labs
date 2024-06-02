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

// DC 0%:  '0' for 0.5 ms,   '1' for 0 ms
// DC 25%: '0' for 0.375 ms, '1' for 0.125 ms
// DC 50%: '0' for 0.25 ms,  '1' for 0.25 ms
// DC 75%: '0' for 0.125 ms, '1' for  ms 0.375
// DC 100%: '0' for 0 ms,     '1' for 0.5 ms

void pwm(int Nfreq){
	int Switches_State = readSWs;
	PWM_driver(Nfreq * (Switches_State - 1) * P7OUT_RATE , Nfreq * (5 - Switches_State) * P7OUT_RATE);
}


  

