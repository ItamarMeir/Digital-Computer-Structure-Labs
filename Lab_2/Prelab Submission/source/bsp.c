#ifdef __MSP430G2553__
#include  "../header/bsp_msp430x2xx.h"    // private library - BSP msp430x2xx layer
#else
#include  "../header/bsp_msp430x4xx.h"    // private library - BSP msp430x4xx layer
#endif
#include  "../header/app.h"    		// private library - APP layer


//-----------------------------------------------------------------------------
//           GPIO congiguration - Lab 1
//
//-----------------------------------------------------------------------------
void GPIOconfig(void){
 // volatile unsigned int i; // in case of while loop usage

  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT

  // LEDs 8-bit Array Port configuration
  LEDsArrPortSel &= ~0xFF;            // GPIO capability
  LEDsArrPortDir |= 0xFF;             // output dir
  LEDsArrPort = 0x00;		      // clear all LEDs

  // Switches Setup
  SWsArrPortSel &= ~0x0F;
  SWsArrPortDir &= ~0x0F;

  // PushButtons Setup
  // --------------------------
  // Lab 1 configuration:
  // P2.0 - P2.3: Inputs with interupts enabled
  // P2.7 - Ouput for a PWM
  // ---------------------


  PBsArrPortSel &= ~0x8F;           // 0xxx0000
  PBsArrPortDir &= ~0x0F;           // xxxx0000 (P2.0 - P2.3: Inputs)
  PBsArrPortDir |= 0x80;            // 1xxxxxx - Set P2.7 - Ouput
  PBsArrIntEdgeSel |= 0x03;  	    // pull-up mode - PB0-1 // Interrupt Edge Select Registers
                                    // Bit = 0: The PxIFGx flag is set with a low-to-high transition
                                    // Bit = 1: The PxIFGx flag is set with a high-to-low transition
  PBsArrIntEdgeSel &= ~0x0C;        // pull-down mode - PB2-3
  PBsArrIntEn |= 0x0F;              // Enabaling P0-2 INT
  PBsArrIntPend &= ~0xFF;           // clear pending interrupts

  _BIS_SR(GIE);                     // enable interrupts globally
}
//-------------------------------------------------------------------------------------
//            Timers congiguration
//-------------------------------------------------------------------------------------
void TIMERconfig(void){
	
	//write here timers congiguration code
}
//-------------------------------------------------------------------------------------
//            ADC congiguration
//-------------------------------------------------------------------------------------
void ADCconfig(void){
	
	//write here ADC congiguration code
}










