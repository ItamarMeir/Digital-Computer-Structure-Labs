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
#define LCD_CTL_DIR        P2DIR
#define LCD_CTL_OUT        P2OUT
#define LCD_EN_PIN         BIT5
#define LCD_RS_PIN         BIT6
#define LCD_RW_PIN         BIT7

// LEDs abstraction
#define LEDsArrPort        P9OUT
#define LEDsArrPortDir     P9DIR
#define LEDsArrPortSel     P9SEL

// Timers abstraction - Timer0 = Timer_A0, Timer1 = Timer_A1
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


//   Generator abstraction            
#define GenPort            P2IN
#define GenPortSel         P2SEL
#define GenPortSel2        P2SEL2
#define GenPortDir         P2DIR
                                   

//   Buzzer abstraction
#define BuzzPortSel        P2SEL
#define BuzzPortSel2       P2SEL2
#define BuzzPortDir        P2DIR
#define BuzzPortOut        P2OUT


// Switches abstraction
#define SWsArrPort         P2IN
#define SWsArrPortDir      P2DIR
#define SWsArrPortSel      P2SEL
#define SWmask             0xF0
#define SW0                0x01
#define SW1                0x02
#define SW2                0x04
#define SW3                0x08



// PushButtons abstraction
#define PBsArrPort	       P2IN
#define PBsArrIntPend	   P2IFG 
#define PBsArrIntEn	       P2IE
#define PBsArrIntEdgeSel   P2IES
#define PBsArrPortSel      P2SEL 
#define PBsArrPortDir      P2DIR 
#define PB0                0x01
#define PB1                0x02
#define PB2                0x04
#define PB3                0x08

// DMA abstraction
#define DMA0_Src_Add       DMA0SA
#define DMA0_Dst_Add       DMA0DA
#define DMA0_Size          DMA0SZ

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

// General bits abstraction
#define BIT0                 0x01
#define BIT1                 0x02
#define BIT2                 0x04
#define BIT3                 0x08
#define BIT4                 0x10
#define BIT5                 0x20
#define BIT6                 0x40
#define BIT7                 0x80

// RGB abstraction
#define RGBArrPortOut      P2OUT
#define RGBArrPortDir      P2DIR
#define RGBArrPortSEL      P2SEL
#define BLUE               BIT0
#define GREEN              BIT1
#define RED                BIT2

extern void GPIOconfig(void);
extern void TIMER0_A0_config(void);
extern void TIMER1_A0_config(void);
extern void ADCconfig(void);

#endif
