#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include <stdlib.h>
#include "stdio.h"

//-------------------------------------------------------------
//                    convert int to string
//-------------------------------------------------------------
void to_string(char *str, unsigned int num){        // convert integer to string
    long tmp = num, len = 0;                     // tmp is a temporary variable to store the number
    int j;      
    while(tmp){                            // calculate the length of the number
        len++;
        tmp /= 10;
    }
    for(j = len - 1; j >= 0; j--){        // convert the number to string
        str[j] = (num % 10) + '0';       // get the last digit of the number
        num /= 10;                      // remove the last digit
    }
    str[len] = '\0';                // add null character at the end of the string
}

//-------------------------------------------------------------
//         State 1: Manual Control of Motor Based Machine 
//-------------------------------------------------------------
void Manual_Motor(){
 
}

//-------------------------------------------------------------
//         State 2: Joystick Based PC Painter
//-------------------------------------------------------------
void Joystick_Painter(){
 
}

//-------------------------------------------------------------
//         State 3: Stepper Motor Calibration 
//-------------------------------------------------------------
void Stepper_Calibration(){
 
}

//-------------------------------------------------------------
//         State 4: Script Mode
//-------------------------------------------------------------
void Script_Mode(){
 
}