#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430g2553.h>          // MSP430x2xx

// *** FAMILY 4 ***
// #include  <msp430xG46x.h>  // MSP430x4xx
// *** FAMILY 4 ***

#define   debounceVal      250
#define   LEDs_SHOW_RATE   0x1FF0  // 62_5ms
#define   P7OUT_RATE       0x0003  // ???

// *** FAMILY 4 - CHANGE TO P9 ***
// LEDs abstraction
#define LEDsArrPort        P1OUT
#define LEDsArrPortDir     P1DIR
#define LEDsArrPortSel     P1SEL
// *** FAMILY 4 - CHANGE TO P9 ***


// Switches abstraction
#define SWsArrPort         P2IN
#define SWsArrPortDir      P2DIR
#define SWsArrPortSel      P2SEL
#define SWmask             0x0F

// PushButtons abstraction
#define PBsArrPort	   P2IN
#define PBsArrIntPend	   P2IFG
#define PBsArrIntEn	   P2IE
#define PBsArrIntEdgeSel   P2IES
#define PBsArrPortSel      P2SEL
#define PBsArrPortDir      P2DIR
#define OutArrPortOut      P2OUT

#define PB0                0x01
#define PB1                0x02
#define PB2                0x04
#define PB3                0x08


extern void GPIOconfig(void);
extern void TIMERconfig(void);
extern void ADCconfig(void);

#endif



