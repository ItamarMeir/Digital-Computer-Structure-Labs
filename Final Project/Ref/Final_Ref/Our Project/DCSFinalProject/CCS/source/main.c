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


  while(1){
	switch(state){

	case state0: //   StepperUsingJoyStick
        //JoyStickRestVr();    // Calibrate Joystick rest values
	    switch(stateStepp){
            case stateAutoRotate:
                Activate_Stepper(500, halfCounterClockwise);
                break;

            case stateJSRotate:
                // while(1){
                //     SampleJoystick();
                //     //int angle = calculateJoystickAngle((int)Vr[1], (int)Vr[0]);

                //     lcd_clear();
                //     lcd_puts("x:");

                //     // Convert Vr[1] (short) to a string
                //     char x_str[10];
                //     sprintf(x_str, "%d", (int)Vr[1]);
                //     lcd_puts(x_str);

                //     lcd_new_line; // This is a function, so add parentheses

                //     lcd_puts("y:");

                //     // Convert Vr[0] (short) to a string
                //     char y_str[10];
                //     sprintf(y_str, "%d", (int)Vr[0]);
                //     lcd_puts(y_str);

                //     timer_delay(500);
                // }
                 StepperUsingJoyStick();
                 ClearJoystickIFG();
                 ClearTXIFG();
                 DisableJoystickInt();
                 DisableTXIE();
                // lcd_clear();
                // lcd_puts("Hello World");
                // delay(20000);
                // lcd_clear();
                // rra_lcd('x');
                stateStepp = stateDefault;
                break;
            case stateDefault:
                EnterLPM();       // Enter LPM0 w/ int until Byte RXed
                break;
            
            case stateStopRotate:
                curr_angle = (int)(360/((double)(max_counter)/ (double)(curr_counter)));
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
                EnterLPM();       // Enter LPM0 w/ int until Byte RXed
                break;

            case stateAutoRotate: // start rotate
                counter = 0;
                Activate_Stepper(800, Clockwise); 
                break;

            case stateStopRotate: // stop and set phi
                calibrate();
                break;
            }
        break;

	case state3:  //Script
        EnableRXIE();                          // Enable USCI_A0 RX interrupt
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

  
  
  
  
  
  
