#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer
#include <stdio.h>
#include <string.h>

extern int  Key;
extern char idiom_recorder[32];
extern const char keyboard[16][4];
extern char data_matrix[10][40];
extern char strMerge[80];
extern char leds[9];
extern char* leds_ptr;
extern char str[];
extern char strMirror[160];
extern void mirror();

extern void idiom_rec();
extern void merge();
extern void shift_leds();

#endif
