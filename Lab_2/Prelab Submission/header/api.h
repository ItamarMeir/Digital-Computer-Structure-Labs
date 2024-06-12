#ifndef _api_H_
#define _api_H_

#include  "../header/hal.h"     // private library - HAL layer

extern void pwm(int);
extern void printSWs2LEDs(void);
extern void printArr2SWs(char Arr[], int size, unsigned int rate);
extern void PWM(unsigned int DC);
extern void write_freq_LCD(unsigned int freq);

#define Timer_A1_start() TA1CTL = SMCLK + ID_0 + MC_1 + TACLR; // SMCLK = 2^20 MHz, /1, Up mode;
#define Timer_A1_stop()  TA1CTL = MC_0; // Stop Timer_A1


#define N_of_measurements 5


#endif







