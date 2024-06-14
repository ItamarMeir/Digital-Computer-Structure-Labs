#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer

//-------------------------------------------------------------
//            Count up to 01:00 and then down to 00:00
//-------------------------------------------------------------
void CountUpDown(void) {
    startTimerA0();
    int i;
    char minDozen = '0';   // ASCII for '0' (tens of minutes)
    char minUnits = '0';   // ASCII for '0' (units of minutes)
    char secDozen = '0';   // ASCII for '0' (tens of seconds)
    char secUnits = '0';   // ASCII for '0' (units of seconds)
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
//            Measure Frequancy from P2.4
//-------------------------------------------------------------
void freqMeas(){
  float SMCLK_Freq = 1048576;
  float Freq;
  float newDiff;
  float oldDiff;
  unsigned int Freq_int;
  startTimerA1();
  Timer1_CCR0 = 0xFFFF; 
  LCD_freq_Template();
  oldDiff = getDiff();
  Freq = SMCLK_Freq / oldDiff;
  Freq *= 1.063; // error correction
  Freq_int = (unsigned int)Freq;
  LCD_freq_print(Freq_int);
  while(state == state1){
    newDiff = getDiff();
    if(oldDiff/newDiff < 0.98 || newDiff/oldDiff < 0.98){
      Freq = SMCLK_Freq / newDiff;
      Freq *= 1.063; // error correction
      Freq_int = (unsigned int)Freq;
      LCD_freq_print(Freq_int);
      oldDiff = newDiff;
    }
  }
  finishTimerA1();
}

//-------------------------------------------------------------
//Generate tone to Buzzer , defined by p1.3 input, output to p2.2
//-------------------------------------------------------------
void tone_generator(){
    unsigned int N_adc;
    int i;
    startTimerA1();
    enableTimerA1_bzr();
    while(state == state3){
        N_adc = 0;
        for(i=0; i<4; i++){
          startADC10(); // Start sampling
          enterLPM(mode0);       // Enter LPM0 w/ interrupt
          finishADC10(); // disable interrupt
          N_adc += ADC_MEM;
        }
        N_adc = (unsigned int) N_adc / 4;
        float m = -0.6;
        float N_fout = m * N_adc + 1048;  // Choose Linear Mapping
        Timer1_CCR0 = (int) N_fout;
        Timer1_CCR1 = (int) N_fout/2;
    }
    disableTimerA1_bzr();
    finishTimerA1();
}
