#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430g2553.h>          // MSP430x2xx

// *** FAMILY 4 ***
// #include  <msp430xG46x.h>  // MSP430x4xx
// *** FAMILY 4 ***

#define   debounceVal      250
#define   LEDs_SHOW_RATE   0x1FF0  // 62_5ms
#define   P7OUT_RATE       0x0004  

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
#define SWmask             0x70

// PushButtons abstraction
#define PBsArrPort	       P1IN
#define PBsArrIntPend	   P1IFG
#define PBsArrIntEn	       P1IE
#define PBsArrIntEdgeSel   P1IES
#define PBsArrPortSel      P1SEL
#define PBsArrPortDir      P1DIR
#define OutArrPortOut      P1OUT

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

// TACTL abstraction
#define TACLK              0x0000
#define ACLK               0x0100
#define SMCLK              0x0200
#define INCLK              0x0300

#define TAIFG              0x0001
#define TAIE               0x0002
#define TACLR              0x0004

#define MC_0_stop          0x0000     // MC_0 - MC_3: Mode control (/0/1/2/3)
#define MC_1_up            0x0010
#define MC_2_continuous    0x0020
#define MC_3_updown        0x0030

#define ID_0               0x0000     // ID_0 - ID_3: Input divider (/1/2/4/8)
#define ID_1               0x0040
#define ID_2               0x0080
#define ID_3               0x00C0


extern void GPIOconfig(void);
extern void TIMERconfig(void);
extern void ADCconfig(void);


#endif



