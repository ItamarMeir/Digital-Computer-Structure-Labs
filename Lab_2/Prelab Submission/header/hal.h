#ifndef _hal_H_
#define _hal_H_


#ifdef __MSP430G2553__
#include  "../header/bsp_msp430x2xx.h"    // private library - BSP msp430x2xx layer
#else
#include  "../header/bsp_msp430x4xx.h"    // private library - BSP msp430x4xx layer
#endif
#include  "../header/app.h"    		// private library - APP layer


extern enum FSMstate state;   // global variable
extern enum SYSmode lpm_mode; // global variable

extern void sysConfig(void);
extern void print2LEDs(unsigned char);
extern void clrLEDs(void);
extern void toggleLEDs(char);
extern void setLEDs(char);
extern char getLEDs();
extern void shlLEDs(char val);
extern unsigned char readSWs(void);
extern void delay(unsigned int);
extern void enterLPM(unsigned char);
extern void incLEDs(char);
extern void enable_interrupts();
extern void disable_interrupts();
extern void resetOutputPin();
extern void setOutputPin();
extern void PWM_driver(int, int);
extern void write_freq_tmp_LCD();
extern void write_time_tmp_LCD();

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

extern __interrupt void Timer_A(void);
extern __interrupt void ADC10_ISR(void);


// --------------------------------------------------------
//              LCD Driver
// --------------------------------------------------------

// #define CHECKBUSY	1  // using this define, only if we want to read from LCD

#ifdef CHECKBUSY
	#define	LCD_WAIT lcd_check_busy()
#else
	#define LCD_WAIT DelayMs(5)
#endif

/*----------------------------------------------------------
  CONFIG: change values according to your port pin selection
------------------------------------------------------------*/
#define LCD_EN(a)	(!a ? (LCD_CTL_OUT &= ~BIT5) : (LCD_CTL_OUT |= BIT5)) // P1.5 is lcd enable pin
#define LCD_EN_DIR(a)	(!a ? (LCD_CTL_DIR &=~ BIT5) : (LCD_CTL_DIR |= BIT5)) // P1.5 pin direction 

#define LCD_RS(a)	(!a ? (LCD_CTL_OUT &=~ BIT6) : (LCD_CTL_OUT |= BIT6)) // P1.6 is lcd RS pin
#define LCD_RS_DIR(a)	(!a ? (LCD_CTL_DIR &= ~BIT6) : (LCD_CTL_DIR |= BIT6)) // P1.6 pin direction  
  
#define LCD_RW(a)	(!a ? (LCD_CTL_OUT &= ~BIT7) : (LCD_CTL_OUT |= BIT7)) // P1.7 is lcd RW pin
#define LCD_RW_DIR(a)	(!a ? (LCD_CTL_DIR &= ~BIT7) : (LCD_CTL_DIR |= BIT7)) // P1.7 pin direction

#define LCD_DATA_OFFSET 0x00 //data pin selection offset for 4 bit mode, variable range is 0-4, default 0 - Px.0-3, no offset
   
// #define LCD_DATA_WRITE	P1OUT
// #define LCD_DATA_DIR	P1DIR
// #define LCD_DATA_READ	P1IN

/*---------------------------------------------------------
  END CONFIG
-----------------------------------------------------------*/
#define FOURBIT_MODE	0x0
#define EIGHTBIT_MODE	0x1
#define LCD_MODE        FOURBIT_MODE
   
#define OUTPUT_PIN      1	
#define INPUT_PIN       0	
#define OUTPUT_DATA     (LCD_MODE ? 0xFF : (0x0F << LCD_DATA_OFFSET))
#define INPUT_DATA      0x00	

#define LCD_STROBE_READ(value)	LCD_EN(1), \
				asm("nop"), asm("nop"), \
				value=LCD_DATA_READ, \
				LCD_EN(0) 

#define	lcd_cursor(x)		lcd_cmd(((x)&0x7F)|0x80)
#define lcd_clear()		lcd_cmd(0x01)
#define lcd_putchar(x)		lcd_data(x)
#define lcd_goto(x)		lcd_cmd(0x80+(x))
#define lcd_cursor_right()	lcd_cmd(0x14)
#define lcd_cursor_left()	lcd_cmd(0x10)
#define lcd_display_shift()	lcd_cmd(0x1C)
#define lcd_home()		lcd_cmd(0x02)
#define cursor_off              lcd_cmd(0x0C)
#define cursor_on               lcd_cmd(0x0F) 
#define lcd_function_set        lcd_cmd(0x3C) // 8bit,two lines,5x10 dots 
#define lcd_new_line            lcd_cmd(0xC0)                                  

extern void lcd_cmd(unsigned char);
extern void lcd_data(unsigned char);
extern void lcd_puts(const char * s);
extern void lcd_init();
extern void lcd_strobe();
extern void DelayMs(unsigned int);
extern void DelayUs(unsigned int);

extern char read_TACCR0(void);
extern char read_TACCR1(void);
/*
 *	Delay functions for HI-TECH C on the PIC18
 *
 *	Functions available:
 *		DelayUs(x)	Delay specified number of microseconds
 *		DelayMs(x)	Delay specified number of milliseconds
*/
// --------------------------------------------------------
//              end of LCD Driver
// --------------------------------------------------------






#endif







