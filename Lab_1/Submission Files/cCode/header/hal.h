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
void resetOutputPin();
void setOutputPin();

extern __interrupt void PBs_handler(void);

#endif







