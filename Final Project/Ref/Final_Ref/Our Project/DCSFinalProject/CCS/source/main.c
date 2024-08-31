#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer
#include  <stdio.h>


enum FSMstate state;
enum Stepperstate stateStepp;
enum RotationState rotation;
enum SYSmode lpm_mode;



void main(void){
  
  state = state0;  // start in idle state on RESET
  stateStepp = stateDefault;
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();     // Configure GPIO, Init ADC
  DisableTXIE();  // Disable USCI_A0 TX interrupt
  ClearTXIFG();   // Clear USCI_A0 TX interrupt flag


  while(1){
	switch(state){
	case state0: //   StepperUsingJoyStick
        
	    switch(stateStepp){
            case stateAutoRotate: 
                stepper_scan(0, 283);
                //Activate_Stepper_Clicks(max_counter/4, 500, Clockwise); // Rotate stepper motor
                //Activate_Stepper(500, Clockwise); // Rotate stepper motor
                break;
        
            case stateJSRotate:
                 JoyStickRestVr();    // Calibrate Joystick rest values
                 StepperUsingJoyStick();
                 ClearJoystickIFG();
                 DisableJoystickInt();
                break;
            case stateDefault:
                EnterLPM();       // Enter LPM0 w/ int until Byte RXed
                break;
            
            case stateStopRotate:
                EnterLPM(); 
                break;
            }
	    break;

	case state1: // Paint
	  //  JoyStickIntEN |= BIT5;
	 //   IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt
        //JoyStickRestVr();   // Calibrate Joystick rest values
	    while (state == state1){
	        JoyStickADC_Painter();
	    }
        JoyStickIntEN &= ~BIT5;
	    break;

	case state2: // Calibrate
        EnableRXIE();                          // Enable USCI_A0 RX interrupt
        switch(stateStepp){
            case stateDefault:
                EnableJoystickInt();
                EnterLPM();       // Enter LPM0 w/ int until Byte RXed
                //stateStepp = stateAutoRotate;
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
        EnableRXIE();                          // Enable USCI_A0 RX interrupt
        switch(stateStepp){
            case stateDefault:
                EnterLPM();       // Enter LPM0 w/ int until Byte RXed
                break;

            case stateAutoRotate: 
                ScriptFunc();
                break;

            case stateStopRotate: 
                
                break;
        }
		
	case state4: //

		break;

    case state5: //

        break;

    case state6: //

        break;

    case state7: //

        break;

    case state8: //

        break;
		
	}
  }
}

  
  
  
  
  
  
