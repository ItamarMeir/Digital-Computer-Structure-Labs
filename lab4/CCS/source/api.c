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
    unsigned int i = 1;             // Starts from Blue (001), Green (010), Red (100)
    while(state == state1){         // state1 is the state for blinking RGB LED
        assign_RGB_value(i);
        i = i << 1;          
        if(i == 8) i = 1;           // reset i to 0 if it reaches 8
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
        if (num >= (1 << 32)-1) num = 0; // reset num to 0 if it reaches 2^32-1            
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
    // CCR value calculation = f_SMCLK / tone_series[j] = ((1 << 20)/tone_series[j])
    unsigned int tone_series[7] = {1000, 1250, 1500, 1750, 2000, 2250, 2500};   // Given tone series
    unsigned int CCR_values[7];                                         // CCR values for the tone series
     unsigned int j = 0;
    for (j = 0; j < 7; j++){
        CCR_values[j] = (unsigned int)((1UL << 20) / tone_series[j]);         // convert the tone series to CCR values
    }
    unsigned int i = 0;
    startTimerA1();
    enableTimerA1_bzr();
    while(state == state3){
        Timer1_CCR0 = CCR_values[i];          // Set value - set the frequency of the buzzer
        Timer1_CCR2 = CCR_values[i++]>>1;    // Reset value - set the duty cycle of the buzzer to 50% and increment i
        if(i == 7) i = 0;                          // reset i to 0 if it reaches 7
        startTimerA0();                             // start timer A0 - delay
        enterLPM(mode0);                            // enter low power mode
        finishTimerA0();                            // finish timer A0
    }
    disableTimerA1_bzr();
    finishTimerA1();
}

//-------------------------------------------------------------
//                         set X value
//-------------------------------------------------------------
// Each timer tick is 8/2^20 = 0.007629 ms. We use up/down mode, so the delay is 2 times the value of CCR0
// define real_time_tick = (8/2^20) * 2 = 2^-16 = 0.015625 ms (include the time for the timer to count up and down)
// CCR0 = (X * 10^-3)/ real_time_tick = X * 2^16 * 10^-3 = X * 65.536 - 1
void set_X(){
    finishTimerA0();                     // finish timer A0
    X = atoi(timerInput);               // convert timerInput to integer by stdlib.h
    X = (unsigned int)(X* 65.536 - 1);  // See calculation above
    if (X >= 0xFFFF){
        X = 0xFFFF;
    }

    TIMER0_A0_config();
}

//-------------------------------------------------------------
//                      Measure POT Value
//-------------------------------------------------------------
void measPOT(){
    double POT_meas;
    char voltage_value[5];
    int i;

    while(state == state5){
        i = 0;
        startADC10();           // start ADC10
        enterLPM(mode0);        // enter low power mode
        finishADC10();          // finish ADC10

        POT_meas = (double)(3.6 * get_ADC_value())/1023;    // convert ADC value to voltage

        lcd_home();
        lcd_puts("Potentiomter");
        lcd_new_line;
        lcd_puts("Meas: ");
        lcd_putchar(((int)POT_meas) + '0');                 // display the integer part of the voltage
        lcd_putchar('.');                                   // display the decimal point
        lcd_putchar(((int)(POT_meas * 10) % 10) + '0');     // display the first decimal digit
        lcd_putchar(((int)(POT_meas * 100) % 10) + '0');   // display the second decimal digit
        lcd_putchar(((int)(POT_meas * 1000) % 10) + '0');  // display the third decimal digit
        lcd_puts(" [v]");                                // display the unit

        startTimerA0();                            // start timer A0 - delay
        enterLPM(mode0);
        finishTimerA0();
    }
}
