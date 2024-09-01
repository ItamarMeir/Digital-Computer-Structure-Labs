#include  "../header/halGPIO.h"     // private library - HAL layer
#include  "../header/flash.h"     // private library - FLASH layer
#include "stdio.h"
#include "stdint.h"
#include "string.h"
//#include "stdlib.h"


// Global Variables
int j=0;
char *ptr1, *ptr2, *ptr3;
short MSBIFG = 0;
short stateIFG = 0; // 0-state changed -> send state(pb pressed)
unsigned int delay_time = 500;
//const unsigned int timer_half_sec = 65535;
unsigned int i = 0;
unsigned int tx_index;
unsigned int tx_length = 0;
const char *tx_str;
char counter_str[4];
//char Vr_pc[8];
short Vr[] = {0, 0}; //Vr[0]=Vry , Vr[1]=Vrx
const short state_changed[] = {1000, 1000}; // send if button pressed - state changed
char RX_str[80];
char file_content[80];
int ExecuteFlag = 0;
int FlashBurnIFG = 0;
int counter = 514;
char step_str[4];
char finish_str[3] = "ACK";
int curr_counter = 0;
int max_counter = 2117;
short finishIFG = 0;
int curr_angle = 0;
int delta_phi = 17;
//unsigned int JoyStickCounter = 0;
short Vr_rest_value[2] = {512, 512};
int SendFlag = 0;  // Flag to differentiate between filename and file content
int burn_index;

//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
	GPIOconfig();
	ADCconfig();
	StopAllTimers();
	UART_init();
    lcd_init();
}
//--------------------------------------------------------------------
//              General Function to Send Data to PC
//--------------------------------------------------------------------
void send_to_PC(const char *input_str) {
    tx_str = input_str;             // Set the global pointer to the input string
    tx_index = 0;
    tx_length = strlen(input_str);  // Determine the length of the input string
    TXBuffer = tx_str[tx_index++];  // Send the first character
    EnableTXIE();                   // Enable USCI_A0 TX interrupt
    EnterLPM();                     // Enter low-power mode with interrupts enabled
    DisableTXIE();
    ClearTXIFG();
}
//--------------------------------------------------------------------
//              Send FINISH to PC
//--------------------------------------------------------------------
void send_finish_to_PC(){
    finishIFG = 1;
    send_to_PC(finish_str);
    finishIFG = 0;
}

//--------------------------------------------------------------------
//              Send degree to PC
//--------------------------------------------------------------------
void send_degree_to_PC(){
    send_to_PC(step_str);
}
//--------------------------------------------------------------------
//              Sample Joystick
//-------------------------------------------------------------------
void SampleJoystick(void) {
    DisableADC();                   // Ensure ADC is disabled before starting
    EnableADC(Vr);                  // Start ADC sampling and conversion
    EnterLPM();                     // Enter LPM0, will exit after ISR
}
//---------------------------------------------------------------------
//            General Function
//---------------------------------------------------------------------
//-----------------------------------------------------------------------
//                  inn to str
//-----------------------------------------------------------------------
void int2str(char *str, unsigned int num){
    int strSize = 0;
    long tmp = num, len = 0;
    int j;
    if (tmp == 0){
        str[strSize] = '0';
        return;
    }
    // Find the size of the intPart by repeatedly dividing by 10
    while(tmp){
        len++;
        tmp /= 10;
    }

    // Print out the numbers in reverse
    for(j = len - 1; j >= 0; j--){
        str[j] = (num % 10) + '0';
        num /= 10;
    }
    strSize += len;
    str[strSize] = '\0';
}

//-----------------------------------------------------------------------
//                     hex to int
//-----------------------------------------------------------------------
uint32_t hex2int(char *hex) {
    uint32_t val = 0;
    int o;
    for(o=0; o<2; o++) {
        // get current character then increment
        uint8_t byte = *hex++;
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9') byte = byte - '0';
        else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;
        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}
//-----------------------------------------------------------------------
//                     hex to char
//-----------------------------------------------------------------------
char hex2char(char hex[]) {
    int value = 0;
    int i;
    for (i = 0; i < 2; i++) {
        if (hex[i] >= '0' && hex[i] <= '9') {
            value = (value << 4) | (hex[i] - '0');
        } else if (hex[i] >= 'A' && hex[i] <= 'F') {
            value = (value << 4) | (hex[i] - 'A' + 10);
        } else if (hex[i] >= 'a' && hex[i] <= 'f') {
            value = (value << 4) | (hex[i] - 'a' + 10);
        }
    }
    return value + '0'; 
}

//-----------------------------------------------------------------------
//                  Motor Go To Position - DELETE
//-----------------------------------------------------------------------
//void motorGoToPosition(uint32_t stepper_degrees, char script_state){
//
//    int clicks_cnt;
//    uint32_t step_counts;
//    uint32_t calc_temp;
//    calc_temp = stepper_degrees * counter;
//    step_counts = (calc_temp / 360); // how much clicks to wanted degree
//
//    //RK code
//    int diff = step_counts - curr_counter;
//    if(0 <= diff){ //move CW
//        for (clicks_cnt = 0; clicks_cnt < diff; clicks_cnt++){
//            curr_counter++;
//            Stepper_clockwise(150);
//            START_TIMERA0(10000);
//            //send data only if FINISH or stepper_deg (state 6)
//            if(script_state == '6'){
//                int2str(step_str, curr_counter);
//                send_degree_to_PC(); }
//        }
//        if (script_state == '7') {
//            int2str(step_str, curr_counter);
//           // sprintf(step_str, "%d", curr_counter);
//            send_degree_to_PC(); }
//        sprintf(step_str, "%s", "FFFF"); // add finish flag
//        send_degree_to_PC();
//    }
//    else{ // move CCW
//        for (clicks_cnt = diff; clicks_cnt < 0; clicks_cnt++){
//            curr_counter--;
//            Stepper_counter_clockwise(150);
//            START_TIMERA0(10000);
//            //send data only if FINISH or stepper_deg (state 6)
//            if(script_state == '6'){
//                int2str(step_str, curr_counter);
//          //      sprintf(step_str, "%d", curr_counter);
//                send_degree_to_PC(); }
//        }
//        if (script_state == '7') {
//            int2str(step_str, curr_counter);
//        //    sprintf(step_str, "%d", curr_counter);
//            send_degree_to_PC(); }
//        sprintf(step_str, "%s", "FFFF"); // add finish flag
//        send_degree_to_PC();
//    }
//}
//------------------------------------------------------------------------
//                      Timer Delay in ms
//------------------------------------------------------------------------
void timer_delay(unsigned int delay_value) {
    unsigned int max_timer_time = 500;  // 0.5 sec delay per overflow based on up mode
    unsigned int full_intervals = delay_value / max_timer_time; // Number of 0.5 second intervals
    unsigned int final_count = (delay_value % max_timer_time) * clk_tmp;  // Remaining time count
    // Run the full intervals using the overflow flag
    while (full_intervals > 0) {
        START_TIMERA1(max_timer_count);  // Start Timer A1 for the maximum count and enter LPM0
        full_intervals--;
    }
    // Run the final interval if there are remaining cycles
    if (final_count > 0) {
        START_TIMERA1(final_count);   // Start Timer A1 for the remaining cycles and enter LPM0
    }
    // Stop the timer after the delay is complete
    StopTimerA1();
}

//------------------------------------------------------------------------
//                      ATAN2- Fixed point 
//------------------------------------------------------------------------
int16_t atan2_fixed_point(int16_t y, int16_t x){
    const int32_t COEFF_A = 45;     
    const int32_t COEFF_B = -56;  // Approximation of -56.24
    const int32_t COEFF_C = 11;   // Approximation of 11.25
    const int16_t RIGHT_ANGLE = 90;
    const int16_t STRAIGHT_ANGLE = 180;
    const int16_t FULL_CIRCLE = 360;

    int16_t angle = 0;
    int32_t ratio, ratio_cubed;
    int16_t y_abs = (y < 0) ? -y : y;

    // Handle special cases
    if (y == 0) {
        return (x >= 0) ? 0 : STRAIGHT_ANGLE;
    }

    if (x >= 0) {
        // Quadrants I and IV
        ratio = (((int32_t)(x - y_abs)) << FIXED_POINT_SHIFT) / ((int32_t)(x + y_abs));
        
        // Calculate ratio^3
        ratio_cubed = ((ratio * ratio) >> FIXED_POINT_SHIFT) * ratio >> FIXED_POINT_SHIFT;
        
        // Calculate angle using polynomial approximation
        angle = (int16_t)(COEFF_A + ((COEFF_B * ratio + COEFF_C * ratio_cubed) >> FIXED_POINT_SHIFT));  // 45 + (-56 * ratio + 11 * ratio^3)
    } else {
        // Quadrants II and III
        ratio = (((int32_t)(x + y_abs)) << FIXED_POINT_SHIFT) / ((int32_t)(y_abs - x));
        
        // Calculate ratio^3
        ratio_cubed = ((ratio * ratio) >> FIXED_POINT_SHIFT) * ratio >> FIXED_POINT_SHIFT;
        
        // Calculate angle using polynomial approximation
        angle = RIGHT_ANGLE + RIGHT_ANGLE + 
                (int16_t)((COEFF_B * ratio + COEFF_C * ratio_cubed) >> FIXED_POINT_SHIFT);
    }

    // Adjust angle for quadrants III and IV
    return (y < 0) ? (FULL_CIRCLE - angle) : angle;
}

//---------------------------------------------------------------------
//                 Flash TXT Files
//---------------------------------------------------------------------
void store_file_in_flash() {
    static int current_file_index = 0;
    if (current_file_index >= MAX_FILES) {
        return;  // Handle the case where the maximum number of files has been reached
    }

    // Select flash address based on current file index
    char* flash_address;
    switch (current_file_index) {
        case 0:
            flash_address = (char*) 0x1000;
            file.file_start_pointers[0] = flash_address;
            break;
        case 1:
            flash_address = (char*) 0x1040;
            file.file_start_pointers[1] = flash_address;
            break;
        case 2:
            flash_address = (char*) 0x1080;
            file.file_start_pointers[2] = flash_address;
            break;
    }
    file.file_sizes[current_file_index] = j;
    file.num_of_files = ++current_file_index;
    // Erase and write to flash
    write_Seg();
    send_finish_to_PC();
}
//---------------------------------------------------------------------
//                 Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
	if (LPM_level == 0x00) 
	  _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01) 
	  _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02) 
	  _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
	else if(LPM_level == 0x03) 
	  _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04) 
	  _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}
//---------------------------------------------------------------------
//                  Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//                  Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}
//---------------------------------------------------------------------
//            Start TimerA0 With counter
//---------------------------------------------------------------------
void START_TIMERA0(unsigned int counter){
    TIMER_A0_config(counter);
    EnterLPM();       // Enter LPM0 w/ interrupt
}
//---------------------------------------------------------------------
//            Start TimerA1 With counter
//---------------------------------------------------------------------
void START_TIMERA1(unsigned int counter){
    TIMER_A1_config(counter);
    EnterLPM();       // Enter LPM0 w/ interrupt
}
// -----------------------------------------------------------------
//                     Polling delays
//------------------------------------------------------------------
//******************************************************************
// Delay usec functions
//******************************************************************
void DelayUs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) asm("nop"); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
// Delay msec functions
//******************************************************************
void DelayMs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
//            Polling based Delay function
//******************************************************************
void delay(unsigned int t){

    volatile unsigned int i;
    for(i=t; i>0; i--);

}
//---------------------------------------------------------------------
//               Interrupt Services Routines
//---------------------------------------------------------------------

//*********************************************************************
//                  TIMER0_A0 ISR - Motor
//*********************************************************************
#pragma vector = TIMER0_A0_VECTOR 
__interrupt void TimerA_ISR(void)
{
    // Increment the global counter
    counter++;
    // Handle Full-Step Rotation (Clockwise or CounterClockwise)
    if (rotation == Clockwise || rotation == CounterClockwise) {
        // Full-step sequence
        const uint8_t full_step_seq[4] = {0x08, 0x01, 0x02, 0x04}; 
        
        // Determine the output based on rotation direction
        if (rotation == Clockwise) {
            StepmotorPortOUT = full_step_seq[step_index]; // {1000, 0001, 0010, 0100}
            curr_counter++;
        } else {
            StepmotorPortOUT = full_step_seq[3 - step_index]; // {0100, 0010, 0001, 1000}
            curr_counter--;
        }

        // Update step index
        step_index = (step_index + 1) % 4;

        // Handle counter reset
        if ((rotation == Clockwise && curr_counter >= max_counter) ||
            (rotation == CounterClockwise && curr_counter <= 0)) {
            if (curr_counter == max_counter || curr_counter == 0) {
                curr_counter = 0;
            } else {
                curr_counter = (rotation == Clockwise) ? 0.5 : max_counter - 0.5;
            }
        }

        if (state != state2) {
            LPM0_EXIT;
        }
    }
    // Handle Half-Step Rotation (Clockwise or CounterClockwise)
    else if (rotation == halfClockwise || rotation == halfCounterClockwise) {
        // Half-step sequence
        const uint8_t half_step_seq[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09}; 
        
        // Determine the output based on rotation direction
        if (rotation == halfClockwise) {
            StepmotorPortOUT = half_step_seq[step_index]; // {0001, 0011, 0010, 0110, 0100, 1100, 1000, 1001}
            curr_counter += 0.5;
        } else {
            StepmotorPortOUT = half_step_seq[7 - step_index]; // {1001, 1000, 1100, 0100, 0110, 0010, 0011, 0001}
            curr_counter -= 0.5;
        }

        // Update step index
        step_index = (step_index + 1) % 8;

        // Handle counter reset
        if ((rotation == halfClockwise && curr_counter == max_counter) ||
            (rotation == halfCounterClockwise && curr_counter < 0)) {
            curr_counter = (curr_counter == max_counter || curr_counter < 0) ? 0 : max_counter;
        }

        if (state != state2) {
            LPM0_EXIT;
        }
    }
    // Handle Invalid Rotation 
    else {
        if (curr_counter == max_counter) {
            curr_counter = 0;
        }
        StopTimerA0();
        step_index = 0;
        LPM0_EXIT;
    }
    LPM0_EXIT;
}
//*********************************************************************
//                  TIMER1_A0 ISR - Delay
//*********************************************************************
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1_A0_ISR (void)
{
    Reset_overflow();  // Reset the overflow flag
    LPM0_EXIT;  
}
//*********************************************************************
//                         ADC10 ISR
//*********************************************************************
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    ClearADCIFG();    // Clear the interrupt flag
    LPM0_EXIT;     // Exit the LPM0 mode
}

//*********************************************************************
//                         RX ISR- OLD
//*********************************************************************
// #if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
// #pragma vector=USCIAB0RX_VECTOR
// __interrupt void USCI0RX_ISR(void)
// #elif defined(__GNUC__)
// void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
// #else
// #error Compiler not supported!
// #endif
// {
//     RX_str[j] = RXBuffer;  // Get Whole string from PC
//     j++;
//     // This if to get the file data. Added 'Z' to the end of the data in the python file, acts like ack
//     if (RX_str[j-1] == 'Z'){
//         j = 0;
//         SendFlag = 0;
//         strcpy(file_content, RX_str);
//      //   ExecuteFlag = 1;
//     }
//     // This if to get the file name
//     if (!SendFlag && RX_str[j-1] == '\x0a'){
//         for (i=0; i < j; i++){
//             file.file_name[i] = RX_str[i];
//         }
//         SendFlag = 1;
//         j = 0;
//     }
//     if (RX_str[j-1] == 'W'){ //pointer for 1st selected file
//         FlashBurnIFG = 1;
//         ptr1 = (char*) 0x1000;
//         file.file_start_pointers[0]=ptr1;
//         file.num_of_files = 1;
//         j = 0;
//     }
//     if (RX_str[j-1] == 'X'){ //pointer for 2nd selected file
//         FlashBurnIFG = 1;
//         ptr2 = (char*) 0x1040;
//         file.file_start_pointers[1]=ptr2;
//         file.num_of_files = 2;
//         j = 0;
//     }
//     if (RX_str[j-1] == 'Y'){ //pointer for 3rd selected file
//         FlashBurnIFG = 1;
//         ptr3 = (char*) 0x1080;
//         file.file_start_pointers[2]=ptr3;
//         file.num_of_files = 3;
//         j = 0; // Added by mg at 1:33 30.7.2022 at night
//     }

//     if (RX_str[j-1] == 'T'){ //index of executed list
//         ExecuteFlag = 1;
//         j = 0; // Added by mg at 1:33 30.7.2022 at night
//         file.num_of_files = 1;
//     }
//     if (RX_str[j-1] == 'U'){
//         ExecuteFlag = 1;
//         j = 0; // Added by mg at 1:33 30.7.2022 at night
//         file.num_of_files = 2;
//     }
//     if (RX_str[j-1] == 'V'){
//         ExecuteFlag = 1;
//         j = 0; // Added by mg at 1:33 30.7.2022 at night
//         file.num_of_files = 3;
//     }


//     // If's for states
//     if (RX_str[0] == MOTOR_STATE) {state = state0; stateStepp=stateDefault; rotateIFG = 0; j = 0;}
//     else if (RX_str[0] == PAINT_STATE) { state = state1; stateStepp=stateDefault; rotateIFG = 0; j = 0;}  //Was p
//     else if (RX_str[0] == CALIB_STATE) { state = state2; stateStepp=stateDefault; rotateIFG = 0; j = 0;}  // Calibration mode
//     else if (RX_str[0] == SCRIPT_STATE) { state = state3; stateStepp=stateDefault; rotateIFG = 0; j = 0;}

//     else if (RX_str[0] == AUTO_ROTATE){ stateStepp = stateAutoRotate; rotateIFG = 1; j = 0;}// Auto Rotate
//     else if (RX_str[0] == STOP_ROTATE){ stateStepp = stateStopRotate; rotation = stop; rotateIFG = 0; j = 0;}// Stop Rotate
//     else if (RX_str[0] == JOYSTICK_ROTATE){ stateStepp = stateJSRotate; j = 0;}// JoyStick Rotatefixed pmsp430


//     LPM0_EXIT;
// }

//*********************************************************************
//                         RX ISR
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    char received_char = RXBuffer;  // Read the received character from the buffer

    if (received_char == STX) { j = 0; }  // Start of message, reset index
    else if (received_char == ETX) {        // End of message
        RX_str[j] = '\0';  // Null-terminate the received string
        if (SendFlag == 0) {
            strncpy(file.file_name[file.num_of_files], RX_str, MAX_FILENAME_LENGTH - 1);
            file.file_name[file.num_of_files][MAX_FILENAME_LENGTH - 1] = '\0';  // Ensure null termination
            SendFlag = 1;
        }  // Store file name, set flag for content
        else {
            store_file_in_flash();
            SendFlag = 0;
        }  // Store file content in flash, reset flag
        j = 0;  // Reset index after processing
    }
    else if (RX_str[0] == 'e' && j == 1) {
            burn_index = received_char - '0'; // Convert char to integer (assuming single digit index)
            ExecuteFlag = 1; // Set flag to execute the selected script
            j = 0;
    }
    else { RX_str[j++] = received_char; }  // Store the received character in the buffer

    if (RX_str[0] == MOTOR_STATE) { state = state0; stateStepp = stateDefault; j = 0; }  // Set motor state
    else if (RX_str[0] == PAINT_STATE) { state = state1; stateStepp = stateDefault; j = 0; }  // Set paint state
    else if (RX_str[0] == CALIB_STATE) { state = state2; stateStepp = stateDefault; j = 0; }  // Set calibration state
    else if (RX_str[0] == SCRIPT_STATE) { state = state3; stateStepp = stateDefault; j = 0; }  // Set script state
    else if (RX_str[0] == AUTO_ROTATE) { stateStepp = stateAutoRotate; j = 0; }  // Set auto rotate state
    else if (RX_str[0] == STOP_ROTATE) { stateStepp = stateStopRotate; rotation = stop; j = 0; }  // Set stop rotate state
    else if (RX_str[0] == JOYSTICK_ROTATE) { stateStepp = stateJSRotate; j = 0; }  // Set joystick rotate state


    LPM0_EXIT;  // Exit low power mode
}

//*********************************************************************
//                         TX ISR
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if (state==state2 || state==state3){
        if (tx_index < tx_length) {
            TXBuffer = tx_str[tx_index++];  // TX next character
        } else {
            tx_index = 0;  // Reset index for the next transmission
            DisableTXIE();  // Disable USCI_A0 TX interrupt
            stateStepp = stateDefault;  // Reset state
            LPM0_EXIT;  // Exit low-power mode
        }
    }

      else if (stateIFG && state == state1){  
          if(MSBIFG) TXBuffer = (state_changed[i++]>>8) & 0xFF;
          else TXBuffer = state_changed[i] & 0xFF;
          MSBIFG ^= 1;

          if (i == 2) {  
              i=0;
              DisableTXIE();                       
              START_TIMERA1(10000);
              stateIFG = 0;
              LPM0_EXIT;
          }
      }
      else if(!stateIFG && state == state1){ 
        if(MSBIFG) TXBuffer = (Vr[i++]>>8) & 0xFF;
        else TXBuffer = Vr[i] & 0xFF;
        MSBIFG ^= 1;

        if (i == 2) {  
            i=0;
            DisableTXIE();                      
            START_TIMERA1(10000);
            LPM0_EXIT;
        }
    }
}

//*********************************************************************
//                        Port1 ISR
//*********************************************************************
#pragma vector=PORT1_VECTOR
  __interrupt void Joystick_handler(void){
      delay(debounceVal);
    if(JoyStickIntPend & BIT5){     // PB at P1.5
        JoyStickIntPend &= ~BIT5;       // Clear the interrupt flag
        if (state == state2){        
            if (stateStepp == stateDefault){    // If the state is default
                stateStepp = stateAutoRotate;   // Change the state to Auto Rotate
            }
            else if (stateStepp == stateAutoRotate){    // If the state is Auto Rotate
                StopTimerA0();
                rotation = stop;
                stateStepp = stateStopRotate;
            }  
        }
        else if (state == state1){
            stateIFG = 1;
        }
    }
    LPM0_EXIT;
  }
//---------------------------------------------------------------------
//                           LCD
//---------------------------------------------------------------------
//******************************************************************
//                  send a command to the LCD
//******************************************************************
void lcd_cmd(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    if (LCD_MODE == FOURBIT_MODE)
    {
        LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
        LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else
    {
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }
}
//******************************************************************
//                    send data to the LCD
//******************************************************************
void lcd_data(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_RS(1);
    if (LCD_MODE == FOURBIT_MODE)
    {
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
            LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
    }
    else
    {
            LCD_DATA_WRITE = c;
            lcd_strobe();
    }

    LCD_RS(0);
}
//******************************************************************
//                write a string of chars to the LCD
//******************************************************************
void lcd_puts(const char * s){
    int i = 0;
    while(*s){                      // write data to LCD up to null
        lcd_data(*s++);             // Write current char and increment pointer
        i++;                        // increment the counter
        if(i == 16 || i == 48){     // check if the counter is equal to 16 or 48
            lcd_new_line;           // move to the next line
        }
    }
}
//******************************************************************
//                     Initialise the LCD
//******************************************************************
void lcd_init(){

    char init_value;

    if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
    else init_value = 0x3F;

    LCD_RS_DIR(OUTPUT_PIN);
    LCD_EN_DIR(OUTPUT_PIN);
    LCD_RW_DIR(OUTPUT_PIN);
    LCD_DATA_DIR |= OUTPUT_DATA;
    LCD_RS(0);
    LCD_EN(0);
    LCD_RW(0);

    DelayMs(15);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayMs(5);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayUs(200);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();

    if (LCD_MODE == FOURBIT_MODE){
        LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
        lcd_strobe();
        lcd_cmd(0x28); // Function Set
    }
    else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots

    lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
    lcd_cmd(0x1); //Display Clear
    lcd_cmd(0x6); //Entry Mode
    lcd_cmd(0x80); //Initialize DDRAM address to zero
    lcd_cmd(0x0C); // remove curser
}
//******************************************************************
//               lcd strobe functions
//******************************************************************
void lcd_strobe(){
  LCD_EN(1);
  asm("NOP");
 // asm("NOP");
  LCD_EN(0);
}
