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
short stateIFG = 1; // 0-state changed -> send state(pb pressed)
int rotateIFG = 1;
unsigned int delay_time = 500;
const unsigned int timer_half_sec = 65535;
unsigned int i = 0;
unsigned int tx_index;
unsigned int tx_length = 0;
const char *tx_str;
char counter_str[4];
short Vr[] = {0, 0}; //Vr[0]=Vry , Vr[1]=Vrx
const short state_changed[] = {1000, 1000}; // send if button pressed - state changed
char stringFromPC[80];
char file_content[80];
int ExecuteFlag = 0;
int FlashBurnIFG = 0;
int SendFlag = 0;
int startRotateLEDs = 0x10;
int* rotateLEDs = &startRotateLEDs;
int counter = 514;
char step_str[4];
char finish_str[3] = "FIN";
int curr_counter = 0;
int max_counter = 2117;
short finishIFG = 0;
int curr_angle = 0;
int delta_phi = 17;
unsigned int JoyStickCounter = 0;
short Vr_rest_value[2] = {512, 512};
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
    // tx_index = 0;
    // TXBuffer = finish_str[tx_index++];
    // EnableTXIE;                        // Enable USCI_A0 TX interrupt
    // __bis_SR_register(LPM0_bits + GIE); // Sleep
    // timer_delay(100);
    // // START_TIMERA0(10000);
    send_to_PC(finish_str);
    finishIFG = 0;
}

//--------------------------------------------------------------------
//              Send degree to PC
//--------------------------------------------------------------------
void send_degree_to_PC(){
    // tx_index = 0;
    // TXBuffer = step_str[tx_index++];
    // EnableTXIE;                        // Enable USCI_A0 TX interrupt
    // __bis_SR_register(LPM0_bits + GIE); // Sleep
    // timer_delay(100);
    // // START_TIMERA0(10000);
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
//---------------------------------------------------------------------
//            Convert Integer to String
//---------------------------------------------------------------------
// void int2str(char *str, unsigned int num) {
//     sprintf(str, "%u", num);  // Use sprintf for easier and safer conversion
// }
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
//                  hex to int
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
//                  Motor Go To Position
//-----------------------------------------------------------------------
void motorGoToPosition(uint32_t stepper_degrees, char script_state){

    int clicks_cnt;
    uint32_t step_counts;
    uint32_t calc_temp;
    calc_temp = stepper_degrees * counter;
    step_counts = (calc_temp / 360); // how much clicks to wanted degree

    //RK code
    int diff = step_counts - curr_counter;
    if(0 <= diff){ //move CW
        for (clicks_cnt = 0; clicks_cnt < diff; clicks_cnt++){
            curr_counter++;
            Stepper_clockwise(150);
            START_TIMERA0(10000);
            //send data only if FINISH or stepper_deg (state 6)
            if(script_state == '6'){
                int2str(step_str, curr_counter);
                send_degree_to_PC(); }
        }
        if (script_state == '7') {
            int2str(step_str, curr_counter);
           // sprintf(step_str, "%d", curr_counter);
            send_degree_to_PC(); }
        sprintf(step_str, "%s", "FFFF"); // add finish flag
        send_degree_to_PC();
    }
    else{ // move CCW
        for (clicks_cnt = diff; clicks_cnt < 0; clicks_cnt++){
            curr_counter--;
            Stepper_counter_clockwise(150);
            START_TIMERA0(10000);
            //send data only if FINISH or stepper_deg (state 6)
            if(script_state == '6'){
                int2str(step_str, curr_counter);
          //      sprintf(step_str, "%d", curr_counter);
                send_degree_to_PC(); }
        }
        if (script_state == '7') {
            int2str(step_str, curr_counter);
        //    sprintf(step_str, "%d", curr_counter);
            send_degree_to_PC(); }
        sprintf(step_str, "%s", "FFFF"); // add finish flag
        send_degree_to_PC();
    }
}
// //---------------------------------------------------------------------
// //                  Motor Go To Position- NEW
// //---------------------------------------------------------------------
// void motorGoToPosition(uint32_t stepper_degrees, char script_state) {
//     uint32_t step_counts = (stepper_degrees * counter) / 360;
//     int diff = step_counts - curr_counter;

//     if (diff >= 0) {  // Move Clockwise
//         for (int clicks_cnt = 0; clicks_cnt < diff; clicks_cnt++) {
//             curr_counter++;
//             Stepper_clockwise(150);
//             START_TIMERA0(10000);
//             if (script_state == '6') {
//                 int2str(step_str, curr_counter);
//                 send_degree_to_PC();
//             }
//         }
//     } else {  // Move Counter-Clockwise
//         for (int clicks_cnt = diff; clicks_cnt < 0; clicks_cnt++) {
//             curr_counter--;
//             Stepper_counter_clockwise(150);
//             START_TIMERA0(10000);
//             if (script_state == '6') {
//                 int2str(step_str, curr_counter);
//                 send_degree_to_PC();
//             }
//         }
//     }

//     if (script_state == '7') {
//         int2str(step_str, curr_counter);
//         send_degree_to_PC();
//     }
//     strcpy(step_str, "FFFF");  // Add finish flag
//     send_degree_to_PC();
// }

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


// Fixed-point multiplication
int16_t fixed_mul(int16_t a, int16_t b) {
    return (int16_t)(((int32_t)a * b) / Q8_8);
}

// Fixed-point division
int16_t fixed_div(int16_t a, int16_t b) {
    
    // Convert Q8.8 to Q16.16 by shifting left by 8 bits
    uint32_t a_long = (uint32_t)a << 8;

    // Perform the division
    uint32_t result_long = a_long / b;

    // Convert back to Q8.8 by taking the lower 16 bits
    uint16_t result = (uint16_t)result_long;

    return result;
}

// Fixed-point multiplication for uint16_t
long fixed_mul_u(long a, long b) {
    return (long)(((a >> 8) * (b >> 8)) >> 0);
}

// Fixed-point division for uint16_t
long fixed_div_u(long a, long b) {
    return (long)(((a) << 7)/(b) << 9);
}
//------------------------------------------------------------------------
//                      ATAN2- Fixed point - returns degrees
//------------------------------------------------------------------------
int16_t atan2_fp(int16_t y_fp, int16_t x_fp)
{
    int32_t coeff_1 = 45;
    int32_t coeff_1b = -56; // 56.24;
    int32_t coeff_1c = 11;  // 11.25
    int16_t coeff_2 = 135;

    int16_t angle = 0;

    int32_t r;
    int32_t r3;

    int16_t y_abs_fp = y_fp;
    if (y_abs_fp < 0)
        y_abs_fp = -y_abs_fp;

    if (y_fp == 0)
    {
        if (x_fp >= 0)
        {
            angle = 0;
        }
        else
        {
            angle = 180;
        }
    }
    else if (x_fp >= 0)
    {
        r = (((int32_t)(x_fp - y_abs_fp)) << MULTIPLY_FP_RESOLUTION_BITS) /
((int32_t)(x_fp + y_abs_fp));

        r3 = r * r;
        r3 =  r3 >> MULTIPLY_FP_RESOLUTION_BITS;
        r3 *= r;
        r3 =  r3 >> MULTIPLY_FP_RESOLUTION_BITS;
        r3 *= coeff_1c;
        angle = (int16_t) (     coeff_1 + ((coeff_1b * r + r3) >>
MULTIPLY_FP_RESOLUTION_BITS)   );
    }
    else
    {
        r = (((int32_t)(x_fp + y_abs_fp)) << MULTIPLY_FP_RESOLUTION_BITS) /
((int32_t)(y_abs_fp - x_fp));
        r3 = r * r;
        r3 =  r3 >> MULTIPLY_FP_RESOLUTION_BITS;
        r3 *= r;
        r3 =  r3 >> MULTIPLY_FP_RESOLUTION_BITS;
        r3 *= coeff_1c;
        angle = coeff_2 + ((int16_t)    (((coeff_1b * r + r3) >>
MULTIPLY_FP_RESOLUTION_BITS))   );
    }

    if (y_fp < 0)
        return (360-angle);     // negate if in quad III or IV
    else
        return (angle);
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
    JoyStickCounter ++;
    counter++;
    //JoyStickCounter++;
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
    stringFromPC[j] = RXBuffer;  // Get Whole string from PC
    j++;
    // This if to get the file data. Added 'Z' to the end of the data in the python file, acts like ack
    if (stringFromPC[j-1] == 'Z'){
        j = 0;
        SendFlag = 0;
        strcpy(file_content, stringFromPC);
     //   ExecuteFlag = 1;
    }
    // This if to get the file name
    if (!SendFlag && stringFromPC[j-1] == '\x0a'){
        for (i=0; i < j; i++){
            file.file_name[i] = stringFromPC[i];
        }
        SendFlag = 1;
        j = 0;
    }
    if (stringFromPC[j-1] == 'W'){ //pointer for 1st selected file
        FlashBurnIFG = 1;
        ptr1 = (char*) 0x1000;
        file.file_ptr[0]=ptr1;
        file.num_of_files = 1;
        j = 0;
    }
    if (stringFromPC[j-1] == 'X'){ //pointer for 2nd selected file
        FlashBurnIFG = 1;
        ptr2 = (char*) 0x1040;
        file.file_ptr[1]=ptr2;
        file.num_of_files = 2;
        j = 0;
    }
    if (stringFromPC[j-1] == 'Y'){ //pointer for 3rd selected file
        FlashBurnIFG = 1;
        ptr3 = (char*) 0x1080;
        file.file_ptr[2]=ptr3;
        file.num_of_files = 3;
        j = 0; // Added by mg at 1:33 30.7.2022 at night
    }

    if (stringFromPC[j-1] == 'T'){ //index of executed list
        ExecuteFlag = 1;
        j = 0; // Added by mg at 1:33 30.7.2022 at night
        file.num_of_files = 1;
    }
    if (stringFromPC[j-1] == 'U'){
        ExecuteFlag = 1;
        j = 0; // Added by mg at 1:33 30.7.2022 at night
        file.num_of_files = 2;
    }
    if (stringFromPC[j-1] == 'V'){
        ExecuteFlag = 1;
        j = 0; // Added by mg at 1:33 30.7.2022 at night
        file.num_of_files = 3;
    }


    // If's for states
    if (stringFromPC[0] == 'm') {state = state0; stateStepp=stateDefault; rotateIFG = 0; j = 0;}
    else if (stringFromPC[0] == 'P') { state = state1; stateStepp=stateDefault; rotateIFG = 0; j = 0;}  //Was p
    else if (stringFromPC[0] == 'C') { state = state2; stateStepp=stateDefault; rotateIFG = 0; j = 0;}  // Calibration mode
    else if (stringFromPC[0] == 's') { state = state3; stateStepp=stateDefault; rotateIFG = 0; j = 0;}

    else if (stringFromPC[0] == 'A'){ stateStepp = stateAutoRotate; rotateIFG = 1; j = 0;}// Auto Rotate
    else if (stringFromPC[0] == 'M'){ stateStepp = stateStopRotate; rotation = stop; rotateIFG = 0; j = 0;}// Stop Rotate
    else if (stringFromPC[0] == 'J'){ stateStepp = stateJSRotate; j = 0;}// JoyStick Rotatefixed pmsp430


    LPM0_EXIT;
}
// //*********************************************************************
// //                         TX ISR
// //*********************************************************************
// #if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
// #pragma vector=USCIAB0TX_VECTOR
// __interrupt void USCI0TX_ISR(void)
// #elif defined(__GNUC__)
// void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
// #else
// #error Compiler not supported!
// #endif
// {
//     if (tx_index < tx_length) {
//         TXBuffer = tx_str[tx_index++];  // TX next character
//     } else {
//         tx_index = 0;  // Reset index for the next transmission
//         DisableTXIE();  // Disable USCI_A0 TX interrupt
//         stateStepp = stateDefault;  // Reset state
//         LPM0_EXIT;  // Exit low-power mode
//     }

// }
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
    if(state == state3 && finishIFG == 1){  // For script
        UCA0TXBUF = finish_str[tx_index++];                 // TX next character

        if (tx_index == sizeof step_str - 1) {   // TX over?
            tx_index=0;
            DisableTXIE;                       // Disable USCI_A0 TX interrupt
            stateStepp = stateDefault;
            LPM0_EXIT;
        }
    }

    if (state == state3 && finishIFG == 0){  // For script
        UCA0TXBUF = step_str[tx_index++];                 // TX next character

        if (tx_index == sizeof step_str - 1) {   // TX over?
            tx_index=0;
            DisableTXIE;                       // Disable USCI_A0 TX interrupt
            stateStepp = stateDefault;
            LPM0_EXIT;
        }
    }
    else if (state==state2 && stateStepp==stateStopRotate){
        if (tx_index < tx_length) {
            TXBuffer = tx_str[tx_index++];  // TX next character
        } else {
            tx_index = 0;  // Reset index for the next transmission
            DisableTXIE();  // Disable USCI_A0 TX interrupt
            stateStepp = stateDefault;  // Reset state
            LPM0_EXIT;  // Exit low-power mode
        }
    }
    else if (stateIFG && state == state1){  // Send Push Button state
        if(MSBIFG) UCA0TXBUF = (state_changed[i++]>>8) & 0xFF;
        else UCA0TXBUF = state_changed[i] & 0xFF;
        MSBIFG ^= 1;

        if (i == 2) {  // TX over?
            i=0;
            DisableTXIE;                       // Disable USCI_A0 TX interrupt
            START_TIMERA0(10000);
            stateIFG = 0;
            LPM0_EXIT;
        }
    }
    else if(!stateIFG && state == state1){ //send data for painter!!
        if(MSBIFG) UCA0TXBUF = (Vr[i++]>>8) & 0xFF;
        else UCA0TXBUF = Vr[i] & 0xFF;
        MSBIFG ^= 1;

        if (i == 2) {  // TX over?
            i=0;
            IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
            START_TIMERA0(10000);
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

      if(JoyStickIntPend & BIT5){          // PB at P1.5           
          JoyStickIntPend &= ~BIT5;
          if (stateStepp == stateDefault){
            stateStepp = stateAutoRotate;
        }
        else if (stateStepp == stateAutoRotate){
            StopTimerA0();
            rotation = stop;
            stateStepp = stateStopRotate;
        }
          LPM0_EXIT;
      }
     
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
