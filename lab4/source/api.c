#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include <stdlib.h>
#include "stdio.h"

//-------------------------------------------------------------
//                    convert int to string
//-------------------------------------------------------------
void to_string(char *str, unsigned int num){
    long tmp = num, len = 0;
    int j;
    while(tmp){
        len++;
        tmp /= 10;
    }
    for(j = len - 1; j >= 0; j--){
        str[j] = (num % 10) + '0';
        num /= 10;
    }
    str[len] = '\0';
}

//-------------------------------------------------------------
//                         Blink RGB
//-------------------------------------------------------------
void blinkRGB(){
    unsigned int i = 0;
    while(state == state1){         // state1 is the state for blinking RGB LED
        RGBArrPortOut = i++;          // increment i to shift the LED color
        if(i == 8) i = 0;           // reset i to 0 if it reaches 8
        startTimerA0();             // start timer A0
        enterLPM(mode0);            // enter low power mode
        finishTimerA0();            // finish timer A0
    }
    RGBArrPortOut = 0x00;
}


//-------------------------------------------------------------
//                       Count up to LCD
//-------------------------------------------------------------
void count(){
    char temp[6];
    while(state == state2){
        to_string(temp, num++);
        lcd_home();
        lcd_puts(temp);
        startTimerA0();
        enterLPM(mode0);
        finishTimerA0();
    }
}

//-------------------------------------------------------------
//                         shift buzzer freq
//-------------------------------------------------------------
void buzzer(){
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
    char num[6];
    int i;

    while(state == state5){
        i = 0;
        startADC10();
        enterLPM(mode0);
        finishADC10();

        POT_meas = ( 53 * ADC_MEM ) >> 4;
        to_string(num, POT_meas);

        lcd_home();
        lcd_puts("Potentiomter Meas:");
        lcd_new_line;
        if(POT_meas <= 999){
            lcd_putchar('0');
        }
        else{
            lcd_putchar(num[i++]);
        }
        lcd_putchar('.');
        if(POT_meas <= 99){
            lcd_putchar('0');
        }
        else{
            lcd_putchar(num[i++]);
        }
        if(POT_meas <= 9){
            lcd_putchar('0');
        }
        else{
            lcd_putchar(num[i++]);
        }
        lcd_putchar(num[i++]);
        lcd_puts(" [v]");

        startTimerA0();
        enterLPM(mode0);
        finishTimerA0();
    }
}
