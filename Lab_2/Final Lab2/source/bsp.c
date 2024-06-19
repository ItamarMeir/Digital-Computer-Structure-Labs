 // private library - BSP layer

#ifdef __MSP430G2553__
#include  "../header/bsp_msp430x2xx.h"
#else
#include  "../header/bsp_msp430x4xx.h"
#endif


//-----------------------------------------------------------------------------  
//           GPIO congiguration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
  
  WDTCTL = WDTHOLD | WDTPW;     // Stop WDT
  
  // LCD configuration
  LCD_DATA_WRITE &= ~0xFF;
  LCD_DATA_DIR |= 0xF0;    // P1.4-P1.7 To Output('1')
  LCD_DATA_SEL &= ~0xF0;   // Bit clear P2.4-P2.7
  LCD_CTL_SEL  &= ~0xE0;   // Bit clear P2.5-P2.7
  
  // Switches Setup
  SWsArrPortSel &= ~0x0F;
  SWsArrPortDir &= ~0x0F;
  
  // Buzzer Setup
  BuzzPortDir |= 0x04;             // P2.2 Output compare - '1'
  BuzzPortSel |= 0x04;             // P2.2 Select = '1'
  BuzzPortSel2 &= ~0x04;
  BuzzPortOut &= ~0x04;            // P2.2 out = '0'
  
  // PushButtons Setup
  PBsArrPortSel &= ~0x0F;
  PBsArrPortDir &= ~0x0F;
  PBsArrIntEdgeSel |= 0x03;  	     // pull-up mode
  PBsArrIntEdgeSel &= ~0x0C;         // pull-down mode
  PBsArrIntEn |= 0x07;
  PBsArrIntPend &= ~0x0F;            // clear pending interrupts 
  
  // Generator Setup
  GenPortDir &= ~0x10;
  GenPortSel |= 0x10;
  GenPortSel2 &= ~0x10;
  
    // PushButton 3 Setup
  PB3PortSel &= ~0x02;
  PB3PortDir &= ~0x02;
  PB3IntEdgeSel |= 0x30;  	     // pull-up mode
  PB3IntEdgeSel &= ~0x04;         // pull-down mode
  PB3IntEn |= 0x02;
  PB3IntPend &= ~0x02;            // clear pending interrupts 
  
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
