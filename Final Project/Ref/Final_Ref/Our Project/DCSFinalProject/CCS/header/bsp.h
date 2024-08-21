#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430g2553.h>          // MSP430x2xx
//#include  <msp430xG46x.h>  // MSP430x4xx


#define   debounceVal      2000
#define     HIGH    1
#define     LOW     0


// LCDs abstraction
#define LCD_DATA_WRITE     P2OUT
#define LCD_DATA_DIR       P2DIR
#define LCD_DATA_READ      P2IN
#define LCD_DATA_SEL       P2SEL
#define LCD_CTL_SEL        P1SEL
#define LCD_CTL_DIR        P1DIR
#define LCD_CTL_OUT        P1OUT
#define LCD_EN_PIN         BIT0
#define LCD_RS_PIN         BIT6
#define LCD_RW_PIN         BIT7


// Joystick abstraction
#define JoyStickPortOUT     P1OUT
#define JoyStickPortSEL     P1SEL
#define JoyStickPortDIR     P1DIR
#define JoyStickPortIN      P1IN
#define JoyStickIntEdgeSel  P1IES
#define JoyStickIntEN       P1IE
#define JoyStickIntPend     P1IFG
#define JoyStickPB          BIT5

// Stepmotor abstraction
#define StepmotorPortOUT     P2OUT
#define StepmotorPortSEL     P2SEL
#define StepmotorPortDIR     P2DIR
//#define StepmotorPortIN      P2IN
//#define StepmotorIntEdgeSel  P2IES
//#define StepmotorIntEN       P2IE
//#define StepmotorIntPend     P2IFG

// General bits abstraction
#define BIT0                 0x01
#define BIT1                 0x02
#define BIT2                 0x04
#define BIT3                 0x08
#define BIT4                 0x10
#define BIT5                 0x20
#define BIT6                 0x40
#define BIT7                 0x80

#define TXLED                BIT0
#define RXLED                BIT6
#define TXD                  BIT2
#define RXD                  BIT1

#define TXBuffer             UCA0TXBUF
#define RXBuffer             UCA0RXBUF

#define ADC_wait while (ADC10CTL1 & ADC10BUSY);   // Wait if ADC10 core is active



extern void GPIOconfig(void);
extern void ADCconfig(void);
extern void TIMER_A0_config(unsigned int counter);
extern void TIMER_A1_config(unsigned int counter);
extern void StopAllTimers(void);
extern void StopTimerA0(void);
extern void StopTimerA1(void);
extern void Reset_overflow(void);
extern void UART_init(void);

extern void EnterLPM(void);
extern void ClearRXIFG(void);
extern void EnableRXIE(void);
extern void DisableRXIE(void);
extern void ClearTXIFG(void);
extern void EnableTXIE(void);
extern void DisableTXIE(void);
extern void DisableJoystickInt(void);
extern void EnableJoystickInt(void);
extern void ClearJoystickIFG(void);
extern void EnableADC(short* DataBufferStart);

#endif



