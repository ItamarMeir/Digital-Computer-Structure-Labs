#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer

extern unsigned int num;
extern int k;

extern void blinkRGB(void);
extern void count(void);
extern void buzzer(void);
extern void set_X(void);
extern void measPOT(void);
extern void print_string();


#endif
