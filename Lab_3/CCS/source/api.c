#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include <stdlib.h>
#include <stdio.h>
//-------------------------------------------------------------
//                      idiom recorder
//-------------------------------------------------------------

void idiom_rec() {
    lcd_puts("Enter Idiom:");   // Display message on LCD

    unsigned int i = 0;         // Variable to store character index
    while (state == state1) {
        unsigned int keyInput;          // Variable to store key input
        unsigned int pressCount = 0;    // Variable to store key press count

        // Enable key interrupts allowing first key input
        en_keypad_interrupts();     // Enable keypad interrupts
        enterLPM(mode0);            // Enter low power mode 0
        disable_keypad_interrupts();    // Disable keypad interrupts

        keyInput = Key;             // Store key input

        while (Key < 16 && Key == keyInput) {       // Loop until key input is valid
            // Break if press count exceeds allowed presses for the key
            if ((keyInput < 12 && pressCount == 1) || (keyInput >= 12 && pressCount == 2)) {  
                break;
            }
            Key = 16;   // Reset key input
            en_keypad_interrupts();     // Enable keypad interrupts
            startTimerA();              // Start timer A
            enterLPM(mode0);            // Enter low power mode 0 to wait for the second key input
            finishTimerA();                // Finish timer A
            disable_keypad_interrupts();    // Disable keypad interrupts

            // Count presses per key
            if (Key == keyInput) {      // If key input is the same as the previous key input
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
            lcd_data(idiom_recorder[i]);        // Display character on LCD

            i++;    // Increment character index
        }
    }
    finishTimerA();     // Finish timer A
}

//-------------------------------------------------------------
//                         merge
//-------------------------------------------------------------

void merge() {
    int i = 0, j = 0, k = 0;    // i - index of substring in indx1, j - index of substring in indx2, k - index of merged string
    int indx1 = -1, indx2 = -1; // Variables to store selected indices
    unsigned int keyInput, prevKeyInput;    // Variables to store key input
    int Size;   // Variable to store size of substring

    lcd_puts("Select 1st str and press *");
    while (state == state2) {

        // Main loop for selecting strings
        while (indx1 == -1 || indx2 == -1){
            en_keypad_interrupts();     // Enable keypad interrupts
            enterLPM(mode0);            // Enter low power mode 0
            disable_keypad_interrupts();    // Disable keypad interrupts
            keyInput = Key;                 // Store key input
            Key = 16;                       // Reset key input

            if (keyInput >= 0 && keyInput <= 2) {       // Adjust key input for usability
                keyInput++;             
            } else if (keyInput >= 8 && keyInput <= 10) {            
                keyInput--; 
            } else if (keyInput == 13) {        
                keyInput = 0;
            } else if (!(keyInput >= 4 && keyInput <= 6) && keyInput != 16 && keyInput != 12) {     // Invalid key input
                lcd_clear();
                lcd_puts("Invalid");
                keyInput = 10;
            }

            if (keyInput < 10) {    // Display selected string on LCD
                lcd_clear();
                lcd_puts(data_matrix[keyInput]);
                prevKeyInput = keyInput;            // Store previous key input
            }

            // Processing selected indices
            if (keyInput == 12 && prevKeyInput < 10) {  // If * is pressed
                if (indx1 == -1) {                  // Selecting 1st string
                    indx1 = prevKeyInput;           // Store selected index
                    prevKeyInput = 10;              // Reset previous key input
                    lcd_clear();
                    lcd_puts("Select 2nd str  and press *");
                } else if (indx2 == -1) {       // Selecting 2nd string
                    indx2 = prevKeyInput;       // Store selected index
                    lcd_clear();                // Clear LCD
                    lcd_puts("done");           // Display message on LCD
                    break;
                }
            }
        }


        // Merging the selected strings using DMA
        while (data_matrix[indx1][i] != '\0' || data_matrix[indx2][j] != '\0'){     // Loop until end of strings
            startDMA();     // Start DMA
            Size = 0;       // Reset size of substring

            // Finding the size of substring in indx1
            while (data_matrix[indx1][i + Size] != '\0' && data_matrix[indx1][i + Size] != ' ') {   // Loop until end of substring
                Size++;
            }

            // i - index of substring in indx1, j - index of substring in indx2, k - index of merged string
            if (Size > 0) {
                if (k > 0) strMerge[k++] = ' ';                     // Add space between words if not the first substring
                DMA0_Src_Add = (void *)(data_matrix[indx1] + i);    // Set source address as the substring in indx1
                DMA0_Dst_Add = (void *)(strMerge + k);              // Set destination address as the merged string
                DMA0_Size = Size;                               // Set size of substring
                triggerDMA();                                    // Trigger DMA
                k += Size;                                // Increment index of merged string
            }

            i += Size;      // Increment index of substring in indx1
            if (data_matrix[indx1][i] == ' ') {     // Skip space
                i++;    // Increment index of substring in indx1
            }

            Size = 0;    // Reset size of substring
            // Finding the size of substring in indx2
            while (data_matrix[indx2][j + Size] != '\0' && data_matrix[indx2][j + Size] != ' ') {   // Loop until end of substring
                Size++;    // Increment size of substring
            }

            if (Size > 0) {    // If size of substring is greater than 0
                if (k > 0) strMerge[k++] = ' ';    // Add space between words if not the first substring
                DMA0_Src_Add = (void *)(data_matrix[indx2] + j);    // Set source address as the substring in indx2
                DMA0_Dst_Add = (void *)(strMerge + k);            // Set destination address as the merged string
                DMA0_Size = Size;                            // Set size of substring
                triggerDMA();                             // Trigger DMA
                k += Size;                           // Increment index of merged string
            }

            j += Size;  // Increment index of substring in indx2
            if (data_matrix[indx2][j] == ' ') {   // Skip space
                j++;        
            }
            stopDMA();  // Stop DMA
        }

        // Displaying the merged string on LCD
        lcd_clear();
        lcd_puts(strMerge);
        enterLPM(mode0);

// -------------- Version of scrolling LCD --------------
        // int str_merge_size = k;     // Store size of merged string
        // if (str_merge_size < 16) {   // If size of merged string is less than 16
        //     lcd_puts(strMerge);    // Display merged string on LCD
        // } else {    // If size of merged string is greater than 16
        //     lcd_puts("Press * to scroll");   // Display message on LCD
        //     int scroll = 0;     // Variable to store scroll index
        //     while (state == state2) {   // Loop until state is state2
        //         en_keypad_interrupts();     // Enable keypad interrupts
        //         enterLPM(mode0);            // Enter low power mode 0
        //         disable_keypad_interrupts();    // Disable keypad interrupts
        //         keyInput = Key;             // Store key input
        //         Key = 16;                   // Reset key input

        //         if (keyInput == 12) {       // If * is pressed
        //             scroll++;               // Increment scroll index
        //             lcd_clear();            // Clear LCD
        //             lcd_puts(strMerge + scroll);    // Display merged string on LCD
        //             if (scroll == str_merge_size - 16) {    // If scroll index is equal to size of merged string - 16
        //                 break;      // Break
        //             }
        //         }
        //     }
        // }
       //-------------------------------------------------------------
    }
}
//-------------------------------------------------------------
//                         shift_leds
//-------------------------------------------------------------


void shift_leds(){
    int i = 0;
    for(i = 0; i < 9; i++){ // Loop through the LEDs array
        if (state != state3)
        {
            break;
        }
        startTimerB();      // Start timer B
        DMA0_Src_Add = (void (*)())leds_ptr;
        DMA0_Dst_Add = (void (*)())&LEDsArrPort;
        DMA0_Size = 0x0001;     // Block size of 1
        startDMA();
        enterLPM(mode0);        // Enter low power mode 0 and wait for timer B to interrupt
        // In timerB interrupt, leds_ptr is incremented and exits LPM
    }
    
    DMA0_Size = 0x0000;
    clear_LEDs();    // Clear LEDs

    // -------------- Version with single transfer DMA --------------
    // char leds[9] = {128,64,32,16,8,4,23,13,40};
    // startTimerB();
    // DMA0_Src_Add = (void (*)())leds;
    // DMA0_Dst_Add = (void (*)())&LEDsArrPort;
    // DMA0_Size = 0x0009;
    // startDMA();
    // enterLPM(mode0);
    // DMA0_Size = 0x0000;
    // LEDsArrPort = 0x00;
    // -------------------------------------------------------------
    
}


