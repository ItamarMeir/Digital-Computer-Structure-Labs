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
//                         Blink RGB
//-------------------------------------------------------------
void blinkRGB(){
    unsigned int i = 0;
    while(state == state1){         // state1 is the state for blinking RGB LED
        assign_RGB_value(i++);          // increment i to shift the LED color
        if(i == 8) i = 0;           // reset i to 0 if it reaches 8
        startTimerA0();             // start timer A0
        enterLPM(mode0);            // enter low power mode
        finishTimerA0();            // finish timer A0
    }
    clear_RGB();
    
}


//-------------------------------------------------------------
//                       Count up to LCD
//-------------------------------------------------------------
void count(){
    char temp[6];
    while(state == state2){
        to_string(temp, num++);     // convert num to string and store it in temp
        lcd_home();                 // move the cursor to the first line
        lcd_puts(temp);             // display the number on the LCD
        startTimerA0();            // start timer A0
        enterLPM(mode0);       // enter low power mode
        finishTimerA0();       // finish timer A0
    }
}

//-------------------------------------------------------------
//                         shift buzzer freq
//-------------------------------------------------------------
void buzzer(){
    //tone series = {1kHz, 1.25kHz, 1.5kHz, 1.75kHz, 2kHz, 2.25kHz, 2.5kHz}
    int tone_series[7] = {1048, 838, 700, 600, 524, 466, 420};
    unsigned int i = 0;
    startTimerA1();
    enableTimerA1_bzr();
    while(state == state3){
        Timer1_CCR0 = (int)tone_series[i];
        Timer1_CCR2 = (int)tone_series[i++]>>1;
        if(i == 7) i = 0;
        startTimerA0();
        enterLPM(mode0);
        finishTimerA0();
    }
    disableTimerA1_bzr();
    finishTimerA1();
}

//-------------------------------------------------------------
//                         set X value
//-------------------------------------------------------------
// Each timer tick is 8/2^20 = 0.007629 ms. We use up/down mode, so the delay is 2 times the value of CCR0
// define real_time_tick = (8/2^20) * 2 = 2^-16 = 0.015625 ms (include the time for the timer to count up and down)
// CCR0 = (X * 10^-3)/ real_time_tick = X * 2^16 * 10^-3 = X * 65.536
void set_X(){
    finishTimerA0();                     // finish timer A0
    X = atoi(timerInput);               // convert timerInput to integer by stdlib.h
    // X = (X*33) >> 5; // = X * 1.024     
    // if(X >= 1024){                      // Maximum daelay possible is 1 sec = 1024 ms
    //     X = 0xFFFF;
    // }
    // else {
    //     X = X << 6;
    // }
    X = (int)(X* 65.536 - 1);
    if (X >= 0xFFFF){
        X = 0xFFFF;
    }

    TIMER0_A0_config();
}

//-------------------------------------------------------------
//                      Measure POT Value
//-------------------------------------------------------------
void measPOT(){
    int POT_meas;
    char voltage_value[6];
    int i;

    while(state == state5){
        i = 0;
        startADC10();           // start ADC10
        enterLPM(mode0);        // enter low power mode
        finishADC10();          // finish ADC10

        POT_meas = ( 53 * get_ADC_value()) >> 4;    // convert ADC value to voltage
        to_string(voltage_value, POT_meas);                   // convert POT_meas to string

        lcd_home();
        lcd_puts("Potentiomter Meas:");
        lcd_new_line;
        if(POT_meas <= 999){                    // display the voltage value on the LCD             
            lcd_putchar('0');                   
        }
        else{
            lcd_putchar(voltage_value[i++]);    
        }
        lcd_putchar('.');
        if(POT_meas <= 99){
            lcd_putchar('0');
        }
        else{
            lcd_putchar(voltage_value[i++]);
        }
        if(POT_meas <= 9){
            lcd_putchar('0');
        }
        else{
            lcd_putchar(voltage_value[i++]);
        }
        lcd_putchar(voltage_value[i++]);
        lcd_puts(" [v]");

        startTimerA0();
        enterLPM(mode0);
        finishTimerA0();
    }
}
