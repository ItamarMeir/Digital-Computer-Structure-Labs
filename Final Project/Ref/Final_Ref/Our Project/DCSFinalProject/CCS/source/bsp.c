#include "../header/bsp.h"    // Private library - BSP layer

//-----------------------------------------------------------------------------  
//           GPIO Configuration
//-----------------------------------------------------------------------------
void GPIOconfig(void) {
    WDTCTL = WDTHOLD | WDTPW;  // Stop WDT

    // JoyStick Configuration (P1.3 - Vrx; P1.4 - Vry; P1.5 - PB)
    JoyStickPortSEL &= ~JoyStickPB;         // P1.5 Sel = '0'
    JoyStickPortDIR &= ~JoyStickPB;         // P1.5 input = '0'
    JoyStickPortOUT &= ~JoyStickPB;         // P1.5 output = '0'
    JoyStickIntEN &= ~JoyStickPB;           // Disable Joystick interrupt
    JoyStickIntEdgeSel |= JoyStickPB;       // P1.5 PB pull-up enabled
    JoyStickIntPend &= ~JoyStickPB;         // Reset Joystick interrupt flag

    // Stepper Motor Configuration (P2.0-P2.3)
    StepmotorPortSEL &= ~(BIT0 + BIT1 + BIT2 + BIT3);  // Clear select bits
    StepmotorPortDIR |= (BIT0 + BIT1 + BIT2 + BIT3);   // Set as output

    // LCD Configuration
    LCD_DATA_WRITE &= ~0xFF;
    LCD_DATA_DIR |= (BIT7 + BIT6 + BIT5 + BIT4);       // Set P2.4-P2.7 to output
    LCD_DATA_SEL &= ~(BIT7 + BIT6 + BIT5 + BIT4);      // Clear select bits
    LCD_CTL_SEL  &= ~(BIT7 + BIT6 + BIT0);             // Clear select bits for P1.7, P1.6, P1.0

    _BIS_SR(GIE);  // Enable global interrupts
}

//-----------------------------------------------------------------------------
//            Timer A0 Configuration
//-----------------------------------------------------------------------------
void TIMER_A0_config(unsigned int counter) {
    TACCR0 = counter;                      // Set counter value
    TACCTL0 = CCIE;                        // Enable capture/compare interrupt
    TA0CTL = TASSEL_2 + MC_1 + ID_3;       // SMCLK, Up mode, Divider /8
    TA0CTL |= TACLR;                       // Clear timer
}

//-----------------------------------------------------------------------------
//            Timer A1 Configuration
//-----------------------------------------------------------------------------
void TIMER_A1_config(unsigned int counter) {
    TA1CCR0 = counter;                     // Set counter value
    TA1CCTL0 = CCIE;                       // Enable capture/compare interrupt
    TA1CTL = TASSEL_2 + MC_1 + ID_3;       // SMCLK, Up mode, Divider /8
    TA1CTL |= TACLR;                       // Clear timer
}

//-----------------------------------------------------------------------------
//            Stop Timers
//-----------------------------------------------------------------------------
void StopTimerA0(void) {
    TA0CTL = MC_0;     // Stop Timer A0
    TA0CTL &= ~TAIFG;  // Clear interrupt flag
}

void StopTimerA1(void) {
    TA1CTL = MC_0;     // Stop Timer A1
    TA1CTL &= ~TAIFG;  // Clear interrupt flag
}

void StopAllTimers(void) {
    StopTimerA0();     // Stop Timer A0
    StopTimerA1();     // Stop Timer A1
}

//-----------------------------------------------------------------------------
//                              UART Initialization
//-----------------------------------------------------------------------------
void UART_init(void) {
    if (CALBC1_1MHZ == 0xFF) {
        while (1);  // Trap CPU if calibration constant is erased
    }

    DCOCTL = 0;                 // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;      // Set DCO frequency to 1MHz
    DCOCTL = CALDCO_1MHZ;

    P1SEL |= BIT1 + BIT2;       // P1.1 = RXD, P1.2 = TXD
    P1SEL2 |= BIT1 + BIT2;      // P1.1 = RXD, P1.2 = TXD

    UCA0CTL1 |= UCSSEL_2;       // Set SMCLK as clock source
    UCA0BR0 = 104;              // Baud rate 9600 with 1MHz clock
    UCA0BR1 = 0x00;
    UCA0MCTL = UCBRS0;          // Modulation UCBRSx = 1

    UCA0CTL1 &= ~UCSWRST;       // Initialize USCI state machine

    ClearRXIFG();
    EnableRXIE();
    ClearTXIFG();
    DisableTXIE();
}

//-----------------------------------------------------------------------------
//            ADC Configuration
//-----------------------------------------------------------------------------
void ADCconfig(void) {
    ADC10CTL0 = ADC10SHT_3 + MSC + ADC10ON + ADC10IE;  // Sample and hold time, Multiple sample/conversion, ADC ON, ADC interrupt enable
    ADC10CTL0 &= ~ADC10IFG;                            // Clear ADC interrupt flag
    ADC10CTL0 &= ~ENC;                                 // Disable ADC

    ADC10CTL1 = INCH_4 + CONSEQ_1 + ADC10SSEL_3;       // INCH4, Sequence of channels, SMCLK
    ADC10AE0 = Vx_Pin + Vy_Pin;                        // P1.3-4 ADC10 option select
    ADC10DTC1 = 0x02;                              // Set block transfer for noise reduction
}
// void ADCconfig(void) {
//     ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON;  // Vcc/Vss as reference, 16 clock cycles, ADC on
//     ADC10CTL1 = INCH_3;                         // Start with the input channel 3 (P1.3)
//     ADC10AE0 |= BIT3 + BIT4;                    // Enable analog input on P1.3 and P1.4
// }

// short readADC(int channel) {
//     ADC10CTL1 = (channel << 12);                // Set the input channel (e.g., INCH_x)
//     ADC10CTL0 |= ENC + ADC10SC;                 // Enable conversion and start conversion
//     while (ADC10CTL1 & ADC10BUSY);              // Wait for conversion to finish
//     return ADC10MEM;                            // Return the result
// }
//-----------------------------------------------------------------------------
//            Flash Configuration
//-----------------------------------------------------------------------------
void configureFlashTiming(void)
{
    // Configure the flash timing generator: MCLK/3
    FCTL2 = FWKEY + FSSEL0 + FN1;
}
//-----------------------------------------------------------------------------
//            Utility Functions
//-----------------------------------------------------------------------------
void EnterLPM(void) {
    __bis_SR_register(LPM0_bits + GIE); // Enter LPM0 with global interrupt enabled
}

void ClearRXIFG(void) {
    IFG2 &= ~UCA0RXIFG;  // Clear USCI_A0 RX interrupt flag
}

void EnableRXIE(void) {
    IE2 |= UCA0RXIE;     // Enable USCI_A0 RX interrupt
}

void DisableRXIE(void) {
    IE2 &= ~UCA0RXIE;    // Disable USCI_A0 RX interrupt
}

void ClearTXIFG(void) {
    IFG2 &= ~UCA0TXIFG;  // Clear USCI_A0 TX interrupt flag
}

void EnableTXIE(void) {
    IE2 |= UCA0TXIE;     // Enable USCI_A0 TX interrupt
}

void DisableTXIE(void) {
    IE2 &= ~UCA0TXIE;    // Disable USCI_A0 TX interrupt
}

void DisableJoystickInt(void) {
    JoyStickIntEN &= ~JoyStickPB; // Disable Joystick interrupt
}

void EnableJoystickInt(void) {
    JoyStickIntEN |= JoyStickPB;  // Enable Joystick interrupt
}

void ClearJoystickIFG(void) {
    JoyStickIntPend &= ~JoyStickPB; // Clear Joystick interrupt flag
}

void EnableADC(short* DataBufferStart) {
        ADC10SA = DataBufferStart;        // Data buffer start (start address of the buffer)
        ADC10CTL0 |= ENC + ADC10SC;       // Sampling and conversion start
}

void DisableADC(void) {
    ADC10CTL0 &= ~ENC;              // Disable ADC
    while (ADC10CTL1 & ADC10BUSY);  // Wait if ADC10 core is active
}

void ClearADCIFG(void) {
    ADC10CTL0 &= ~ADC10IFG;     // Clear the interrupt flag
}

void Reset_overflow(void) {
    if (TA1CTL & TAIFG) {  // Check if overflow flag is set
        TA1CTL &= ~TAIFG;  // Clear overflow flag
    }
}