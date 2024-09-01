#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer
#include  <stdio.h>


enum FSMstate state;
enum Stepperstate stateStepp;
enum RotationState rotation;
enum SYSmode lpm_mode;



void main(void){
  
  state = state0;  // start in idle state on RESET
  stateStepp = stateStopRotate; // in state0 the default state is stop
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();     // Configure GPIO, Init ADC
  DisableTXIE();  // Disable USCI_A0 TX interrupt
  ClearTXIFG();   // Clear USCI_A0 TX interrupt flag


  while(1){
    EnableRXIE();                          // Enable USCI_A0 RX interrupt
	switch(state){
	case state0: //state0 -  Manual Control
        
	    switch(stateStepp){
            case stateAutoRotate: 
                Activate_Stepper(500, Clockwise); // Rotate stepper motor
                break;
        
            case stateJSRotate:
                 JoyStickRestVr();    // Calibrate Joystick rest values
                 StepperUsingJoyStick(); // Rotate stepper motor using Joystick
                 ClearJoystickIFG();     // Clear Joystick interrupt flag
                 DisableJoystickInt();  // Disable Joystick interrupt
                break; 

            case stateStopRotate:
                EnterLPM(); 
                break;
            }
	    break;

	case state1: // state1 - Paint
	    while (state == state1){
	        JoyStickADC_Painter();
	    }
        DisableJoystickInt();
	    break;

	case state2: // Calibrate
        switch(stateStepp){
            case stateDefault:
                EnableJoystickInt();
                EnterLPM();       // Enter LPM0 w/ int until Byte RXed
                break;

            case stateAutoRotate: // start rotate
                counter = 0;
                Activate_Stepper(500, Clockwise);
                //stateStepp = stateStopRotate;
                break;

            case stateStopRotate: // stop and set phi
                DisableJoystickInt();
                calibrate();
                stateStepp = stateDefault;
                break;
            }
        break;

	case state3:  //Script
	    while ( state == state3){
	        ScriptFunc();
	    }
		break;
	}
  }
}

  
  
  
  
  
  
