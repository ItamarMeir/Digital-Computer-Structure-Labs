#ifndef _api_H_
#define _api_H_

#include  "../header/hal.h"     // private library - HAL layer


extern void printSWs2LEDs(void);
extern void printArr2SWs(char Arr[], int size, unsigned int rate);
extern void PWM(unsigned int DC);

#endif







