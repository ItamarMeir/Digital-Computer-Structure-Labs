#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer

extern unsigned int num;

extern void to_string(char *str, unsigned int num);

extern void Script_Mode();
extern void Stepper_Calibration();
extern void Joystick_Painter();
extern void Manual_Motor();


#endif
