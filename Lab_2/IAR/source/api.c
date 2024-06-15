#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer


//-------------------------------------------------------------
//            Measure Frequency from P2.4
//-------------------------------------------------------------
void freqMeas() {
    const float SMCLK_Freq = 1048576.0;
    const float errorCorrection = 1.06315;
    const float epsilon = 0.002;
    float Freq;
    float newDiff, oldDiff;
    unsigned int Freq_int;

    startTimerA1();
    Timer1_CCR0 = 0xFFFF;
    LCD_freq_Template();

    oldDiff = getDiff();
    Freq = (SMCLK_Freq / oldDiff) * errorCorrection;
    Freq_int = (unsigned int)Freq;
    LCD_freq_print(Freq_int);

    while (state == state1) {
        newDiff = getDiff();
        if (((oldDiff > newDiff) ? (oldDiff - newDiff) : (newDiff - oldDiff)) / oldDiff > epsilon) {
            Freq = (SMCLK_Freq / newDiff) * errorCorrection;
            Freq_int = (unsigned int)Freq;
            LCD_freq_print(Freq_int);
            oldDiff = newDiff;
        }
    }

    finishTimerA1();
}
//-------------------------------------------------------------
//            Count up from 00:00 to 59:59 and SW0 on/off
//-------------------------------------------------------------
void CountUp(void) {
    startTimerA0();
    char minDozen = '0';   
    char minUnits = '0';   
    char secDozen = '0';
    char secUnits = '0';   
    char buffer[6];        // Buffer for displaying the time "MM:SS"
    buffer[2] = ':';       // Static colon separator
    buffer[5] = '\0';      // Null-terminate the buffer

    lcd_home();
    lcd_puts("00:00");

    while (state == state2) {
        unsigned char switchState = readSWs();

        if (switchState == 0) {
            // Stop counting and display the current time
            enterLPM(mode0);
            continue;
        }

        // Update the time display
        buffer[0] = minDozen;
        buffer[1] = minUnits;
        buffer[3] = secDozen;
        buffer[4] = secUnits;

        // Null-terminate the buffer
        buffer[5] = '\0';

        lcd_home();
        lcd_puts(buffer);

        // Update seconds
        if (secUnits == '9') {
            secUnits = '0';
            if (secDozen == '5') {  // If tens of seconds reach '5'
                secDozen = '0';
                // Update minutes
                if (minUnits == '9') {
                    minUnits = '0';
                    if (minDozen == '5') {  // If tens of minutes reach '5'
                        minDozen = '0';     // Reset minutes to '00'
                        lcd_home();
                        lcd_puts("00:00");
                    } else {
                        minDozen++;
                    }
                } else {
                    minUnits++;
                }
            } else {
                secDozen++;
            }
        } else {
            secUnits++;
        }

        // Enter low power mode
        enterLPM(mode0);
    }

    finishTimerA0();
}

//-------------------------------------------------------------
// Generate tone for Buzzer, input by p1.3 output to p2.2
//-------------------------------------------------------------
void tone_generator() {
    unsigned int N_adc;
    int i;
    float m = -0.6;
    float n = 1048;
    float N_fout;

    startTimerA1();
    enableTimerA1_bzr();

    while (state == state3) {
        N_adc = 0;
        
        // Accumulate ADC values
        for (i = 0; i < 4; i++) {
            startADC10();        // Start sampling
            enterLPM(mode0);     // Enter Low Power Mode 0 with interrupt
            finishADC10();       // Disable interrupt
            N_adc += ADC_MEM;    // Add ADC memory value
        }

        // Calculate the average ADC value
        N_adc /= 4;

        // Apply linear mapping to calculate the frequency output
        N_fout = m * N_adc + n;

        // Set Timer1 CCR0 and CCR1
        Timer1_CCR0 = (int)N_fout;
        Timer1_CCR1 = (int)(N_fout / 2);
    }

    disableTimerA1_bzr();
    finishTimerA1();
}