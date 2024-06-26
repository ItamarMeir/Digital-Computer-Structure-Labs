#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer

extern int  Key;
extern char idiom_recorder[32];
extern const char keyboard[16][4];
extern char data_matrix[10][40];
extern char strMerge[80];
extern void idiom_rec();
extern void merge();
extern void shift_leds();

#endif
