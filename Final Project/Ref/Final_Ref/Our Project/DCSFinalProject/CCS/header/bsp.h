#ifndef _BSP_H_
#define _BSP_H_

#include <stdint.h>
#include <msp430g2553.h>  // MSP430x2xx

// General Definitions
#define Q8_8                256  // Scaling factor for Q8.8 format
#define debounceVal         2000
#define HIGH                1
#define LOW                 0

// LCD Abstraction
#define LCD_DATA_WRITE      P2OUT
#define LCD_DATA_DIR        P2DIR
#define LCD_DATA_READ       P2IN
#define LCD_DATA_SEL        P2SEL
#define LCD_CTL_SEL         P1SEL
#define LCD_CTL_DIR         P1DIR
#define LCD_CTL_OUT         P1OUT
#define LCD_EN_PIN          BIT0
#define LCD_RS_PIN          BIT6
#define LCD_RW_PIN          BIT7

// Joystick Abstraction
#define JoyStickPortOUT     P1OUT
#define JoyStickPortSEL     P1SEL
#define JoyStickPortDIR     P1DIR
#define JoyStickPortIN      P1IN
#define JoyStickIntEdgeSel  P1IES
#define JoyStickIntEN       P1IE
#define JoyStickIntPend     P1IFG
#define JoyStickPB          BIT5

// Stepper Motor Abstraction
#define StepmotorPortOUT    P2OUT
#define StepmotorPortSEL    P2SEL
#define StepmotorPortDIR    P2DIR

// ADC Abstraction
#define Vx_Pin              BIT3
#define Vy_Pin              BIT4

// UART Definitions
#define TXLED               BIT0
#define RXLED               BIT6
#define TXD                 BIT2
#define RXD                 BIT1
#define TXBuffer            UCA0TXBUF
#define RXBuffer            UCA0RXBUF

// Function Declarations
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
extern void DisableADC(void);
extern void ClearADCIFG(void);
extern short readADC(int channel);
extern void configureFlashTiming(void);




#endif  // _BSP_H_