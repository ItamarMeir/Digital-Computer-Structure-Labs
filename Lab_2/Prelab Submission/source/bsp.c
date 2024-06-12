#ifdef __MSP430G2553__
  #include  "../header/bsp_msp430x2xx.h"    // private library - BSP msp430x2xx layer
#else
  #include  "../header/bsp_msp430x4xx.h"    // private library - BSP msp430x4xx layer
#endif
#include  "../header/app.h"    		// private library - APP layer


//-----------------------------------------------------------------------------
//           GPIO congiguration - Lab 2
// P2.0: SW0 - Input for state 2
// P1.0 - P1.2: PB0 - PB2
// P1.3 Analog Input (A3)
// P1.4 - P1.7: LCD data
// P2.5 - P2.7: LCD control
// P2.4: Generator output (for input capture)
// P2.2: PWM output (output compare) connected to buzzer and CH2 of the oscilloscope

// Port description:
// ~~ Port 1 ~~
// P1.0 - P1.2: PushButtons
// P1.3: Analog Input (A3)
// P1.4 - P1.7: LCD data

// ~~ Port 2 ~~
// P2.0: SW0 input
// P2.2: PWM output
// P2.4: Generator output
// P2.5 - P2.7: LCD control

//-----------------------------------------------------------------------------
void GPIOconfig(void){
 // volatile unsigned int i; // in case of while loop usage

  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT

  // // LEDs 8-bit Array Port configuration
  // LEDsArrPortSel &= ~0xFF;            // GPIO capability
  // LEDsArrPortDir |= 0xFF;             // output dir
  // LEDsArrPort = 0x00;		      // clear all LEDs

  // Switches Setup
  SWsArrPortSel &= ~SW0;     // for more than one switch: SWsArrPortSel &= ~(SW0 | SW1 | SW2 | SW3);
  SWsArrPortDir &= ~SW0;    // for more than one switch: SWsArrPortDir &= ~(SW0 | SW1 | SW2 | SW3);

  // PushButtons Setup

  PBsArrPortSel &= ~(PB0 | PB1 | PB2);           // xxxxx000
  PBsArrPortDir &= ~(PB0 | PB1 | PB2);           // xxxxx000 (P1.0 - P1.2: Inputs)
 // PBsArrPortDir |= 0x80;                         // 1xxxxxx - Set P2.7 - Ouput
  PBsArrIntEdgeSel |= (PB0 | PB1);  	      // pull-up mode - PB0-1 // Interrupt Edge Select Registers
                                    // Bit = 0: The PxIFGx flag is set with a low-to-high transition
                                    // Bit = 1: The PxIFGx flag is set with a high-to-low transition
  PBsArrIntEdgeSel &= ~(PB2);        // pull-down mode - PB2-3
  PBsArrIntEn |= (PB0 | PB1 | PB2);              // Enabaling P0-2 INT
  PBsArrIntPend &= ~0xFF;           // clear pending interrupts

  
  
  // Generator Setup
  //From the table at CCIx p2.4
  GenPortDir &=  ~BIT4;               // P2.4 Input Capture = '1'
  GenPortSel |=  BIT4;              // P2.4 Select = '1'

  // Buzzer Setup
  BuzzPortDir |= BIT2;             // P2.2 Output compare - '1'
  BuzzPortSel |= BIT2;             // P2.2 Select = '1'
  BuzzPortOut &= ~BIT2;             // P2.2 out = '0'

  _BIS_SR(GIE);                     // enable interrupts globally
}
//-------------------------------------------------------------------------------------
//            Timers congiguration
//-------------------------------------------------------------------------------------
void TIMERconfig(void){
// TimerA1 configuration
TA1CCTL2 = CM_1 + CCIS_0 + CAP + SCS + CCIE; // Rising edge, CCI0A, Capture mode, Synchronous


}


//-------------------------------------------------------------------------------------
//            ADC congiguration
//-------------------------------------------------------------------------------------

void ADCconfig(void){
	
	//write here ADC congiguration code
}









