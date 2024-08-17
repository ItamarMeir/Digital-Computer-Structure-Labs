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
    
    // LPM0 - sleep mode
	  case state0:
	      enable_UARTRX_interrupts();          // Enable USCI_A0 RX interrupt
	      enterLPM(lpm_mode);                   // enter low power mode
		  break;
	
    // 	Manual Control of Motor Based Machine 
	  case state1:
	      enable_UARTRX_interrupts();          
	      Manual_Motor();                           
		  break;

    // Joystick Based PC Painter	 
	  case state2:
	      enable_UARTRX_interrupts();     
	      Joystick_Painter();                                 
		  break;

    // Stepper Motor Calibration            
	  case state3:
	      enable_UARTRX_interrupts();     
	      Stepper_Calibration();                                
	      break;

    // Script Mode        
	  case state4:                                  
        
        Script_Mode();
		break;   

	}
  }
}
