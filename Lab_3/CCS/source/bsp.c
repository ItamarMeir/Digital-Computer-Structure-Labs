 // private library - BSP layer

#ifdef __MSP430FG4619__
#include  "../header/bsp_msp430x4xx.h"
#else
#include  "../header/bsp_msp430x2xx.h"
#endif

//-----------------------------------------------------------------------------  
//           GPIO configuration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
  
  WDTCTL = WDTHOLD | WDTPW;     // Stop WDT
  
  // LCD configuration
  LCD_DATA_WRITE &= ~0xFF;
  LCD_DATA_DIR |= 0xF0;    // P1.4-P1.7 To Output ('1')
  LCD_DATA_SEL &= ~0xF0;   // Bit clear P2.4-P2.7
  LCD_CTL_SEL  &= ~0xE0;   // Bit clear P2.5-P2.7

  // LED configuration
  LEDsArrPort = 0x00;
  LEDsArrPortDir |= 0xFF;
  LEDsArrPortSel &= ~0xFF;

  // Keypad Setup
  KeypadPortSel &= ~0xFF;
  KeypadPortDIR = 0x0F;     // 10.0-10.3 output, 10.4-10.7 input
  KeypadPortOUT = 0x00;     // CLR output

  // Keypad IRQ Setup
  KeypadIRQPortSel &= ~BIT1;
  KeypadIRQPortDir &= ~BIT1;             // P2.1 input
  KeypadIRQIntEdgeSel |= BIT1;           // pull-up mode  P2.1 - '1'
  KeypadIRQIntEn &= ~BIT1;               // P2.1 - '0'
  KeypadIRQIntPend &= ~0xFF;             // clear pending interrupts P2.1
  
  // PushButtons Setup
  PBsArrPortSel &= ~(PB0 + PB1 + PB2); //Add PB3
  PBsArrPortDir &= ~(PB0 + PB1 + PB2);
  PBsArrIntEdgeSel |= (PB0 + PB1);  	        // pull-up mode
  PBsArrIntEdgeSel &= ~(PB2 + PB3);            // pull-down mode
  PBsArrIntEn |= (PB0 + PB1 + PB2); //Add PB3
  PBsArrIntPend &= ~(PB0 + PB1 + PB2 + PB3);            // clear pending interrupts

  
  _BIS_SR(GIE);                         // enable interrupts globally
}                             

//------------------------------------------------------------------------------------- 
//             Timers configuration
//-------------------------------------------------------------------------------------
void TIMER0_A0_config(void){
    Timer0_CCR0 = 0xFFFF;
    Timer0_CTL = TASSEL_2 + MC_3 + ID_3 + TACLR;  //  select: 2 - SMCLK ; control: 3 - Up/Down  ; divider: 3 - /8
    Timer0_CTL &= ~TAIE;

    Timer1_CCR0 = 0xFFFF;
    Timer1_CTL = TBSSEL_2 + MC_2 + ID_3 + TBCLR;  //  select: 2 - SMCLK ; control: 2 - UP  ; divider: 3 - /8
    Timer1_CTL &= ~TBIE;

} 

//------------------------------------------------------------------------------------- 
//            DMA configuration
//-------------------------------------------------------------------------------------
void DMA_config(void){
    DMA0CTL &= ~DMAIE;
}
