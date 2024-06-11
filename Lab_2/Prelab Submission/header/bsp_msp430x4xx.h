#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430xG46x.h>  // MSP430x4xx


#define   debounceVal      250
#define   LEDs_SHOW_RATE   0x1FF0  // 62_5ms
#define   P7OUT_RATE       0x0003  // ???

// LEDs abstraction
#define LEDsArrPort        P9OUT
#define LEDsArrPortDir     P9DIR
#define LEDsArrPortSel     P9SEL


// Switches abstraction
#define SWsArrPort         P6IN
#define SWsArrPortDir      P6DIR
#define SWsArrPortSel      P6SEL
#define SWmask             0x0F

// PushButtons abstraction
#define PBsArrPort	       P2IN
#define PBsArrIntPend	   P2IFG
#define PBsArrIntEn	       P2IE
#define PBsArrIntEdgeSel   P2IES
#define PBsArrPortSel      P2SEL
#define PBsArrPortDir      P2DIR
#define OutArrPortOut      P2OUT

// LCDs abstraction
#define LCD_DATA_WRITE     P1OUT
#define LCD_DATA_DIR       P1DIR
#define LCD_DATA_READ      P1IN
#define LCD_DATA_SEL       P1SEL
#define LCD_CTL_SEL        P2SEL

// Generator abstraction
#define GenPort            P2IN
#define GenPortSel         P2SEL
#define GenPortDir         P2DIR
#define GenPortOut         P2OUT

// Buzzer abstraction
#define BuzzPortSel        P2SEL
#define BuzzPortDir        P2DIR
#define BuzzPortOut        P2OUT


#define PB0                0x01
#define PB1                0x02
#define PB2                0x04
#define PB3                0x08

#define SW0                0x01
#define SW1                0x02
#define SW2                0x04
#define SW3                0x08

#define BIT0               0x01
#define BIT1               0x02
#define BIT2               0x04
#define BIT3               0x08
#define BIT4               0x10
#define BIT5               0x20
#define BIT6               0x40
#define BIT7               0x80


extern void GPIOconfig(void);
extern void TIMERconfig(void);
extern void ADCconfig(void);

#endif



