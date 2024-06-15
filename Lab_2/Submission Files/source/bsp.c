 // private library - BSP layer

#ifdef __MSP430G2553__
#include  "../header/bsp_msp430x2xx.h"
#else
#include  "../header/bsp_msp430x4xx.h"
#endif

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

//-----------------------------------------------------------------------------  
//           GPIO congiguration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
  
  WDTCTL = WDTHOLD | WDTPW;     // Stop WDT
  
  // LCD configuration
  LCD_DATA_WRITE &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
  LCD_DATA_DIR |= (BIT4 | BIT5 | BIT6 | BIT7);    // P1.4-P1.7 To Output('1')
  LCD_DATA_SEL &= ~(BIT4 | BIT5 | BIT6 | BIT7);   // Bit clear P2.4-P2.7
  LCD_CTL_SEL  &= ~(BIT5 | BIT6 | BIT7);   // Bit clear P2.5-P2.7
  
  // Switches Setup
  SWsArrPortSel &= ~(SW0 | SW1 | SW2 | SW3);
  SWsArrPortDir &= ~(SW0 | SW1 | SW2 | SW3);
  
  // Buzzer Setup
  BuzzPortDir |= BIT2;             // P2.2 Output compare - '1'
  BuzzPortSel |= BIT2;             // P2.2 Select = '1'
  BuzzPortSel2 &= ~BIT2;
  BuzzPortOut &= ~BIT2;            // P2.2 out = '0'
  
  // PushButtons Setup
  PBsArrPortSel &= ~(PB0 | PB1 | PB2 | PB3);
  PBsArrPortDir &= ~(PB0 | PB1 | PB2 | PB3);
  PBsArrIntEdgeSel |= (PB0 | PB1); 	                  // pull-up mode
  PBsArrIntEdgeSel &= ~(PB2 | PB3);                   // pull-down mode
  PBsArrIntEn |= (PB0 | PB1 | PB2);
  PBsArrIntPend &= ~(PB0 | PB1 | PB2 | PB3);          // clear pending interrupts 
  
  // Generator Setup
  GenPortDir &= ~BIT4;
  GenPortSel |= BIT4;
  GenPortSel2 &= ~BIT4;
  
  _BIS_SR(GIE);                     // enable interrupts globally
}                             

//------------------------------------------------------------------------------------- 
//             Timers congiguration
//-------------------------------------------------------------------------------------
void TIMER0_A0_config(void){
    Timer0_CCR0 = 0xFFFF;
    Timer0_CTL |= TASSEL_2 + MC_0 + ID_3 + TACLR;  //  select: 2 - SMCLK ; control: 0 - hold  ; divider: 3 - /8
    Timer0_CTL &= ~TAIE;
} 

void TIMER1_A0_config(void){
    Timer1_CCR0 = 0xFFFF;
    Timer1_CTL |= TASSEL_2 + MC_0 + ID_0 + TACLR;
    Timer1_CCTL2 = CAP + CM_1 + SCS + CCIS_0;
    Timer1_CCTL2 &= ~CCIE;
}  

//------------------------------------------------------------------------------------- 
//            ADC congiguration 
//-------------------------------------------------------------------------------------
void ADCconfig(void){
      ADC_CTL0 = ADC10SHT_2 + ADC10ON+ SREF_0 + ADC10IE;  // 16*ADCLK+ Turn on, ref to Vcc and Gnd, and Enable Interrupt
      ADC_CTL1 = INCH_3 + ADC10SSEL_3;     // Input A3 and SMCLK
      ADC_AE0 |= BIT3;                      // P1.3 ADC
}
