#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430g2553.h>          // MSP430x2xx


#define   debounceVal      250


// LCDs abstraction
#define LCD_DATA_WRITE     P1OUT
#define LCD_DATA_DIR       P1DIR
#define LCD_DATA_READ      P1IN
#define LCD_DATA_SEL       P1SEL
#define LCD_CTL_SEL        P2SEL

// RGB abstraction
#define RGBArrPortOut      P2OUT
#define RGBArrPortDir      P2DIR
#define RGBArrPortSEL      P2SEL

// LDR abstraction
//#define LDRArrPort         P1OUT
//#define LDRArrPortDir      P1DIR
//#define LDRArrPortSel      P1SEL

// Timers abstraction
#define Timer0_CCR0        TACCR0
#define Timer0_CTL         TACTL
#define Timer1_CCR0        TA1CCR0
#define Timer1_CCR1        TA1CCR1
#define Timer1_CCR2        TA1CCR2
#define Timer1_CTL         TA1CTL
#define Timer1_CCTL1       TA1CCTL1
#define Timer1_CCTL2       TA1CCTL2

// ADC abstraction
#define ADC_CTL0           ADC10CTL0
#define ADC_CTL1           ADC10CTL1
#define ADC_AE0            ADC10AE0
#define ADC_MEM            ADC10MEM

// Buzzer abstraction
#define BuzzPortSel        P2SEL
#define BuzzPortDir        P2DIR
#define BuzzPortOut        P2OUT

// UART abstraction
#define TXLED              BIT0
#define RXLED              BIT6
#define TXD                BIT2
#define RXD                BIT1

extern void GPIOconfig(void);
extern void TIMER0_A0_config(void);
extern void TIMER1_A0_config(void);
extern void ADCconfig(void);
extern void UARTconfig(void);
extern unsigned X;

#endif
