#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer
#include  <stdio.h>


enum FSMstate state;
enum Stepperstate stateStepp;
enum RotationState rotation;
enum SYSmode lpm_mode;

double ang;

void main(void){
  
  state = state0;  // start in idle state on RESET
  stateStepp = stateDefault;
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();     // Configure GPIO, Init ADC


  while(1){
	switch(state){

	case state0: //   StepperUsingJoyStick
	    switch(stateStepp){
            case stateAutoRotate:
                Activate_Stepper(200, CounterClockwise);
                break;

            case stateJSRotate:
                //counter = 514;
                StepperUsingJoyStick();

                ClearJoystickIFG();
                ClearTXIFG();
                DisableJoystickInt();
                DisableTXIE();
                break;
            case stateDefault:
                EnterLPM();       // Enter LPM0 w/ int until Byte RXed
                break;
            
            case stateStopRotate:
                ang = (int)(360/((double)(max_counter)/ (double)(curr_counter)));
                EnterLPM(); 
                break;
            }
	    break;

	case state1: // Paint
	    JoyStickIntEN |= BIT5;
	 //   IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt
	    while (state == state1){
	        JoyStickADC_Painter();
	    }
        JoyStickIntEN &= ~BIT5;
	    break;

	case state2: // Calibrate
        EnableRXIE();                          // Enable USCI_A0 RX interrupt

        switch(stateStepp){
            case stateDefault:
                EnterLPM();       // Enter LPM0 w/ int until Byte RXed
                break;

            case stateAutoRotate: // start rotate
                counter = 0;
                Activate_Stepper(200, Clockwise); 
                break;

            case stateStopRotate: // stop and set phi
                calibrate();
                break;
            }
        break;

	case state3:  //Script
        IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
	    while ( state == state3){
	        ScriptFunc();
	    }
		break;
		
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

  
  
  
  
  
  
