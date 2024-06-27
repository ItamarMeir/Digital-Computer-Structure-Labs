#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include <stdlib.h>


//-------------------------------------------------------------
//                      idiom recorder
//-------------------------------------------------------------

void idiom_rec() {
    lcd_puts("Enter Idiom:");   // Display message on LCD

    unsigned int i = 0;         // Variable to store character index
    while (state == state1) {
        unsigned int keyInput;          // Variable to store key input
        unsigned int pressCount = 0;    // Variable to store key press count

        // Enable key interrupts
        KeypadIRQIntEn |= BIT1;     // Enable key interrupts
        enterLPM(mode0);            // Enter low power mode 0
        KeypadIRQIntEn &= ~BIT1;    // Disable key interrupts

        keyInput = Key;             // Store key input

        while (Key < 16 && Key == keyInput) {       // Loop until key input is valid
            // Break if press count exceeds allowed presses for the key
            if ((keyInput < 12 && pressCount == 1) || (keyInput >= 12 && pressCount == 2)) {  
                break;
            }
            Key = 16;   // Reset key input
            KeypadIRQIntPend &= ~BIT1;  // Clear key interrupt pending flag
            KeypadIRQIntEn |= BIT1;    // Enable key interrupts
            startTimerA();              // Start timer A
            enterLPM(mode0);            // Enter low power mode 0
            finishTimerA();                // Finish timer A
            KeypadIRQIntEn &= ~BIT1;    // Disable key interrupts

            // Count presses per key
            if (Key == keyInput) {      // If key input is valid
                pressCount++;               // Increment press count
            }
        }
        // If valid key input and press count within limits, display character on LCD
        if (keyInput < 16 && pressCount < 3) {
            if (i == 0)         // Clear LCD if first character
                lcd_clear();
            else if (i == 16)   // Move to next line if 16 characters are displayed
                lcd_new_line;
            if (i == 32)        // Break if 32 characters are displayed
                break;

            // Display the character based on keyInput and pressCount
            idiom_recorder[i] = keyboard[keyInput][pressCount];
            lcd_data(idiom_recorder[i]);

            i++;
        }
    }
    finishTimerA();
}

//-------------------------------------------------------------
//                         merge
//-------------------------------------------------------------

void merge() {
    int i = 0, j = 0, k = 0;
    int indx1 = -1, indx2 = -1;
    unsigned int keyInput, prevKeyInput;
    int Size;

    lcd_puts("Select 1st str  and press *");
    while (state == state2) {

        // Main loop for selecting strings
        while (indx1 == -1 || indx2 == -1){
            KeypadIRQIntEn |= BIT1;
            enterLPM(mode0);
            KeypadIRQIntEn &= ~BIT1;
            keyInput = Key;
            Key = 16;

            // Adjusting key input for usability
            if (keyInput >= 0 && keyInput <= 2) {
                keyInput++;
            } else if (keyInput >= 8 && keyInput <= 10) {
                keyInput--;
            } else if (keyInput == 13) {
                keyInput = 0;
            } else if (!(keyInput >= 4 && keyInput <= 6) && keyInput != 16 && keyInput != 12) {
                lcd_clear();
                lcd_puts("Invalid");
                keyInput = 10;
            }

            if (keyInput < 10) {
                lcd_clear();
                lcd_puts(data_matrix[keyInput]);
                prevKeyInput = keyInput;
            }

            // Processing selected indices
            if (keyInput == 12 && prevKeyInput < 10) {
                if (indx1 == -1) {
                    indx1 = prevKeyInput;
                    prevKeyInput = 10;
                    lcd_clear();
                    lcd_puts("Select 2nd str  and press *");
                } else if (indx2 == -1) {
                    indx2 = prevKeyInput;
                    lcd_clear();
                    lcd_puts("done");
                    break;
                }
            }
        }


        // Merging the selected strings using DMA
        while (data_matrix[indx1][i] != '\0' || data_matrix[indx2][j] != '\0'){
            startDMA();
            Size = 0;

            // Finding the size of substring in indx1
            while (data_matrix[indx1][i + Size] != '\0' && data_matrix[indx1][i + Size] != ' ') {
                Size++;
            }

            if (Size > 0) {
                if (k > 0) strMerge[k++] = ' ';
                DMA0_Src_Add = (void *)(data_matrix[indx1] + i);
                DMA0_Dst_Add = (void *)(strMerge + k);
                DMA0_Size = Size;
                triggerDMA();
                k += Size;
            }

            i += Size;
            if (data_matrix[indx1][i] == ' ') {
                i++;
            }

            Size = 0;
            // Finding the size of substring in indx2
            while (data_matrix[indx2][j + Size] != '\0' && data_matrix[indx2][j + Size] != ' ') {
                Size++;
            }

            if (Size > 0) {
                if (k > 0) strMerge[k++] = ' ';
                DMA0_Src_Add = (void *)(data_matrix[indx2] + j);
                DMA0_Dst_Add = (void *)(strMerge + k);
                DMA0_Size = Size;
                triggerDMA();
                k += Size;
            }

            j += Size;
            if (data_matrix[indx2][j] == ' ') {
                j++;
            }
            stopDMA();
        }

        // Displaying the merged string on LCD
        lcd_clear();
        lcd_puts(strMerge);
        enterLPM(mode0);
    }
}
//-------------------------------------------------------------
//                         shift_leds
//-------------------------------------------------------------

void shift_leds(){
    char leds[9] = {128,64,32,16,8,4,23,13,40};
    startTimerB();
    DMA0_Src_Add = (void (*)())leds;
    DMA0_Dst_Add = (void (*)())&LEDsArrPort;
    DMA0_Size = 0x0009;
    startDMA();
    enterLPM(mode0);
    DMA0_Size = 0x0000;
    LEDsArrPort = 0x00;
}


