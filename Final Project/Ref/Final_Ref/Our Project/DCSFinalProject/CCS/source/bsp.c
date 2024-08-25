#include  "../header/bsp.h"    // private library - BSP layer

//-----------------------------------------------------------------------------  
//           GPIO configuration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT

  // JoyStick Configuration  P1.3 - Vrx; P1.4 - Vry; P1.5 - PB
  // P1.3-P1.4 - X(Don't care) for Sel, Dir According the dataSheet For A3,A4 input Select For ADC
  JoyStickPortSEL &= ~JoyStickPB;  // P1.5 Sel = '0'
  JoyStickPortDIR &= ~JoyStickPB;  // P1.5 input = '0'
  JoyStickPortOUT &= ~JoyStickPB;  //
  JoyStickIntEN &= ~JoyStickPB; // P1.5 Joystick interrrupt disabled = '0'
  JoyStickIntEdgeSel |= JoyStickPB; // P1.5 PB pull-up - '1'
  JoyStickIntPend &= ~JoyStickPB; // Reset Int IFG - '0'

  // Stepmotor Configuration
  StepmotorPortSEL &= ~(BIT0+BIT1+BIT2+BIT3);  // P2.0-P2.3 Sel = '0'
  StepmotorPortDIR |= BIT0+BIT1+BIT2+BIT3;  // P2.0-P2.3 output = '1'

    // LCD configuration
  LCD_DATA_WRITE &= ~0xFF;
  LCD_DATA_DIR |= BIT7 + BIT6 + BIT5 + BIT4;    // P2.4-P2.7 To Output ('1')
  LCD_DATA_SEL &= ~(BIT7 + BIT6 + BIT5 + BIT4);   // Bit clear P2.4-P2.7
  LCD_CTL_SEL  &= ~(BIT7 + BIT6 + BIT0);   // Bit clear P1.7, P1.6, P1.0

  _BIS_SR(GIE);                     // enable interrupts globally
}

//-------------------------------------------------------------------------------------
//            Timer A0  configuration
//-------------------------------------------------------------------------------------
void TIMER_A0_config(unsigned int counter){
    TACCR0 = counter;                      // Set the counter value for Timer A0
    TACCTL0 = CCIE;                        // Enable capture/compare interrupt for Timer A0
    TA0CTL = TASSEL_2 + MC_1 + ID_3;       // Select SMCLK (TASSEL_2), Up mode (MC_1), and Divider /8 (ID_3)
    TA0CTL |= TACLR;                       // Clear the timer
}
//-------------------------------------------------------------------------------------
//            Timer A1  configuration
//-------------------------------------------------------------------------------------
void TIMER_A1_config(unsigned int counter) {
    TA1CCR0 = counter;                      // Set the counter value for Timer A1
    TA1CCTL0 = CCIE;                        // Enable capture/compare interrupt for Timer A1
    TA1CTL = TASSEL_2 + MC_1 + ID_3;        // Select SMCLK (TASSEL_2), Up mode (MC_1), and Divider /8 (ID_3)
    TA1CTL |= TACLR;                        // Clear the timer
}
//-------------------------------------------------------------------------------------
//            Stop A0 Timer
//-------------------------------------------------------------------------------------
void StopTimerA0(void) {
    TA0CTL = MC_0;     // Halt Timer A0 by setting mode control to "Stop"
    TA0CTL &= ~TAIFG;  // Clear any pending Timer A0 interrupt flags
}
//-------------------------------------------------------------------------------------
//            Stop A1 Timer
//-------------------------------------------------------------------------------------
void StopTimerA1(void) {
    TA1CTL = MC_0;     // Halt Timer A1 by setting mode control to "Stop"
    TA1CTL &= ~TAIFG;  // Clear any pending Timer A1 interrupt flags
}
//-------------------------------------------------------------------------------------
//            Stop All Timers
//-------------------------------------------------------------------------------------
void StopAllTimers(void) {
    StopTimerA0();  // Halt Timer A0
    StopTimerA1();  // Halt Timer A1
}

//-------------------------------------------------------------------------------------
//                              UART init
//-------------------------------------------------------------------------------------
void UART_init(void){
    if (CALBC1_1MHZ==0xFF)                  // If calibration constant erased
      {
        while(1);                               // do not load, trap CPU!!
      }
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
    DCOCTL = CALDCO_1MHZ;

 //   P2DIR = 0xFF;                             // All P2.x outputs
 //   P2OUT = 0;                                // All P2.x reset
    P1SEL |= BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    P1SEL2 |= BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
   // P1DIR |= RXLED + TXLED;
    P1OUT &= 0x00;

    UCA0CTL1 |= UCSSEL_2;                     // CLK = SMCLK
  //  UCA0BR0 = 52;  // For 8Mhz
    UCA0BR0 = 104;
    UCA0BR1 = 0x00;                           //
    UCA0MCTL = UCBRS0;               //
  //  UCA0MCTL = 0x10|UCOS16; //UCBRFx=1,UCBRSx=0, UCOS16=1
    UCA0CTL1 &= ~UCSWRST;        // **Initialize USCI state machine**
    IFG2 &= ~UCA0RXIFG;  // Clear USCI_A0 RX interrupt flag             
    IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt
    IFG2 &= ~UCA0TXIFG;  // Clear USCI_A0 TX interrupt flag
    IE2 &= ~UCA0TXIE;       // Disable USCI_A0 TX interrupt
}


//-------------------------------------------------------------------------------------
//            ADC configuration
//-----------------------------------------------------------------------------------
void ADCconfig(void){       // Vy P1.4(A4), Vx P1.3(A3)
    ADC10CTL0 = ADC10SHT_3 + MSC + ADC10ON + ADC10IE;   // Sample and hold time - 64 CLKs (prevent noise), Multiple sample and conversion, 
                                                        // ADC ON, ADC interrupt enable
    ADC10CTL0 &= ~ADC10IFG;                           // Clear ADC interrupt flag
    ADC10CTL0 &= ~ENC;                                // Disable ADC

    ADC10CTL1 = INCH_4 + CONSEQ_1 + ADC10SSEL_3;            // INCH4 - A4 highest highest channel for a sequence of conversions, 
                                                            // Sequence of channels one conversion, SMCLK

    ADC10AE0 = Vx_Pin + Vy_Pin;                         // P1.3-4 ADC10 option select
    ADC10DTC1 = DTC_transfers;                         // (=2) conversions for each block transfer, reduce noise.
    
}


void EnterLPM(void){
    __bis_SR_register(LPM0_bits + GIE); // Sleep
}

void ClearRXIFG(void){
    IFG2 &= ~UCA0RXIFG;  // Clear USCI_A0 RX interrupt flag
}

void EnableRXIE(void){
    IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt
}

void DisableRXIE(void){
    IE2 &= ~UCA0RXIE; // Disable USCI_A0 RX interrupt
}

void ClearTXIFG(void){
    IFG2 &= ~UCA0TXIFG;  // Clear USCI_A0 TX interrupt flag
}

void EnableTXIE(void){
    IE2 |= UCA0TXIE; // Enable USCI_A0 TX interrupt
}

void DisableTXIE(void){
    IE2 &= ~UCA0TXIE; // Disable USCI_A0 TX interrupt
}

void DisableJoystickInt(void){
    JoyStickIntEN &= ~JoyStickPB; // Disable Joystick interrupt
}

void EnableJoystickInt(void){
    JoyStickIntEN |= JoyStickPB; // Enable Joystick interrupt
}

void ClearJoystickIFG(void){
    JoyStickIntPend &= ~JoyStickPB; // Clear Joystick pending interrupt
}

void DisableADC(void){
    ADC10CTL0 &= ~ENC;                       // Disable ADC
    ADC_wait;                         // Wait if ADC10 core is active
}



void Reset_overflow(void) {
    if (TA1CTL & TAIFG) {  // Check if the overflow flag is set
        TA1CTL &= ~TAIFG;  // Clear the overflow flag
    }
}