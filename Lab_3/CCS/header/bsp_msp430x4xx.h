#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430xG46x.h>


#define   debounceVal      250


// LCDs abstraction
#define LCD_DATA_WRITE     P1OUT
#define LCD_DATA_DIR       P1DIR
#define LCD_DATA_READ      P1IN
#define LCD_DATA_SEL       P1SEL
#define LCD_CTL_SEL        P2SEL

// LEDs abstraction
#define LEDsArrPort        P9OUT
#define LEDsArrPortDir     P9DIR
#define LEDsArrPortSel     P9SEL

// Timers abstraction
#define Timer0_CCR0        TACCR0
#define Timer0_CTL         TACTL
#define Timer1_CCR0        TBCCR0
#define Timer1_CTL         TBCTL

// DMA abstraction
#define DMA0_Src_Add       DMA0SA
#define DMA0_Dst_Add       DMA0DA
#define DMA0_Size          DMA0SZ


// PushButtons abstraction
#define PBsArrPort	       P1IN
#define PBsArrIntPend	   P1IFG
#define PBsArrIntEn	       P1IE
#define PBsArrIntEdgeSel   P1IES
#define PBsArrPortSel      P1SEL
#define PBsArrPortDir      P1DIR
#define PB0                0x01
#define PB1                0x02
#define PB2                0x04
#define PB3                0x08

//  Keypad abstraction
#define KeypadPortSel         P10SEL
#define KeypadPortDIR         P10DIR
#define KeypadPortOUT         P10OUT
#define KeypadPortIN          P10IN
#define KeypadIRQPort         P2IN
#define KeypadIRQIntPend      P2IFG
#define KeypadIRQIntEn        P2IE
#define KeypadIRQIntEdgeSel   P2IES
#define KeypadIRQPortSel      P2SEL
#define KeypadIRQPortDir      P2DIR
#define KeypadIRQPortOut      P2OUT

extern void GPIOconfig(void);
extern void TIMER0_A0_config(void);
extern void DMA_config(void);

#endif
