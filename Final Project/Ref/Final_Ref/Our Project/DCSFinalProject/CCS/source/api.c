
#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include  "../header/flash.h"     // private library - FLASH layer
#include "stdio.h"
#include "math.h"

int flag_script = 1;
int16_t Vrx = 0;
int16_t Vry = 0;
char x;
char y;
int step_index = 0;
unsigned int JoyStick_Error = 0;
int AVG_Vr[2] = {0, 0};
static int delay_value = 50; // Default delay value in units of 10ms


// //-------------------------------------------------------------
// //                Stepper Using JoyStick
// //-------------------------------------------------------------      
void StepperUsingJoyStick(){
    short phi_calculated;
    curr_counter = 0;
    while (state==state0 && stateStepp==stateJSRotate){
        AVG_Vr[0] = 0;
        AVG_Vr[1] = 0;
        int i;
        for (i = 0; i < Number_of_Samples; i++){
            JoyStickADC_Steppermotor();
            AVG_Vr[0] += Vr[0];
            AVG_Vr[1] += Vr[1];
        }
        AVG_Vr[0] /= Number_of_Samples;
        AVG_Vr[1] /= Number_of_Samples;

        if(ABS(AVG_Vr[0] - Vr_rest_value[0]) > 20*JoyStick_Error || ABS(AVG_Vr[1] - Vr_rest_value[1]) > 20*JoyStick_Error){
            //phi_calculated = SCALE_FACTOR * (unsigned long)calculateJoystickAngle((int)AVG_Vr[1], (int)AVG_Vr[0]);
            short Vrx = AVG_Vr[1] - Vr_rest_value[1];
            short Vry = AVG_Vr[0] - Vr_rest_value[0];
            //phi_calculated = atan2_fp(Vry, Vrx);
            phi_calculated = atan2_fixed_point(Vry, Vrx);
            char phi[10];
            int2str(phi, phi_calculated);
            lcd_clear();
            lcd_puts("Angle: ");
            lcd_puts(phi);
            GotoAngle((unsigned long) phi_calculated * (unsigned long)SCALE_FACTOR);
        }
    }
        clear_LCD();
        
    }

//-------------------------------------------------------------
//              Activate Joy Stick
//-------------------------------------------------------------
void JoyStickADC_Steppermotor(){  // Sample the Joystick and save the values in Vr[]
    DisableADC();                     // Disable ADC
    EnableADC(Vr);                    // Start ADC
    EnterLPM();                       // LPM0, ADC10_ISR will force exit
}
//-------------------------------------------------------------
//                Stepper Motor Calibration
//-------------------------------------------------------------
void calibrate(){
    max_counter = counter;
    curr_counter = 0;
    sprintf(counter_str, "%d", max_counter);
    send_to_PC(counter_str);
    delta_phi = (unsigned long)360 * (unsigned long)SCALE_FACTOR / (unsigned long)max_counter;
}
//-------------------------------------------------------------
//              Activate Stepper Motor
//-------------------------------------------------------------
void Activate_Stepper(int speed_Hz, int Rot_state){
    rotation = Rot_state;
    step_index = 0;
    START_TIMERA0(speed_Hz);
    while (rotation == Rot_state) {
        EnterLPM(); // Sleep
    }           
}
//-------------------------------------------------------------
//              Activate Stepper Motor # Clicks
//-------------------------------------------------------------
void Activate_Stepper_Clicks(int clicks, int speed_Hz, int Rot_state){  
    if (clicks <= 0) return;
    rotation = Rot_state;
    int iter = 0;
    START_TIMERA0(speed_Hz);
    for (iter = 0; iter < clicks; iter++){
        EnterLPM(); // Sleep
    }
    rotation = stop;
    StopTimerA0();
}
//-------------------------------------------------------------
//               Rotate Motor to Angle
//-------------------------------------------------------------
void GotoAngle(unsigned long angle){ // assume input angle is scaled by SCALE_FACTOR and positive
    unsigned long phi_current = ((unsigned long)curr_counter * (unsigned long)delta_phi) % (unsigned long)360000; // Current angle in scaled degrees
    unsigned long mod_angle = angle % 360000;   // Wanted angle in scaled degrees
    if (mod_angle == 0 && phi_current > 180000) mod_angle = 360000; // Special case for 0 degrees 
    unsigned long diff = 0; // Absolute difference between the current angle and the wanted angle
    // ABS(phi_current - mod_angle)
    if (mod_angle > phi_current){            
        diff = mod_angle - phi_current;     
    }
    else{
        diff = phi_current - mod_angle;
    }
    unsigned int clicks_needed = (diff / (unsigned long)delta_phi) % max_counter;   // Number of clicks needed to reach the wanted angle
    int i = 0;
    if (mod_angle > phi_current){               // Wanted angle is bigger than current angle
        if (clicks_needed > max_counter / 2){   // The case where it is faster to go counter clockwise
            Activate_Stepper_Clicks(max_counter - clicks_needed, 500, CounterClockwise);
        }
        else{                                // The case where it is faster to go clockwise
            Activate_Stepper_Clicks(clicks_needed, 500, Clockwise);
        }
    }
    else{                                    // Wanted angle is smaller than current angle
        if (clicks_needed > max_counter / 2){  // The case where it is faster to go clockwise
            Activate_Stepper_Clicks(max_counter - clicks_needed, 500, Clockwise);
        }
        else{                             // The case where it is faster to go counter clockwise
            Activate_Stepper_Clicks(clicks_needed, 500, CounterClockwise);
        }
    }
}

//-------------------------------------------------------------
//               Rotate Motor ClockWise to Angle
//-------------------------------------------------------------
void GotoAngleClockWise(unsigned long angle) {
    // Declare variables to hold the results in memory
    volatile unsigned long phi_current;
    volatile unsigned long mod_angle;
    volatile unsigned long diff;
    volatile unsigned int clicks_needed;


    // Calculate the current angle in scaled degrees
    phi_current = ((unsigned long)curr_counter * (unsigned long)delta_phi) % (unsigned long)360000;

    // Calculate the target angle modulo 360000
    mod_angle = angle % 360000;

    // Special case for 0 degrees
    if (mod_angle == 0 && phi_current > 180000) {
        mod_angle = 360000;
    }

    // Calculate the difference between the current angle and the target angle
    if (mod_angle > phi_current) {       
        diff = mod_angle - phi_current;
    } else {
        // Handle the case where the desired angle is smaller
        diff = 360000 - (phi_current - mod_angle);
    }

    // Calculate the number of steps (clicks) needed to reach the target angle
    clicks_needed = (unsigned int)((diff / delta_phi) % max_counter);

    // Activate the stepper motor to move the calculated number of clicks
    Activate_Stepper_Clicks(clicks_needed, 500, Clockwise);
}
//-------------------------------------------------------------
//                Joystick Calibration
//-------------------------------------------------------------
void JoyStickRestVr(){
    Vr_rest_value[0] = 0;
    Vr_rest_value[1] = 0;
    int i;
    for (i = 0; i < Number_of_Samples; i++){
        JoyStickADC_Steppermotor();
        Vr_rest_value[0] += Vr[0];
        Vr_rest_value[1] += Vr[1];
    }
    Vr_rest_value[0] /= Number_of_Samples;
    Vr_rest_value[1] /= Number_of_Samples;

    AVG_Vr[0] = 0;
    AVG_Vr[1] = 0;
    for (i = 0; i < Number_of_Samples; i++){
        JoyStickADC_Steppermotor();
        AVG_Vr[0] += Vr[0];
        AVG_Vr[1] += Vr[1];
    }
    AVG_Vr[0] /= Number_of_Samples;
    AVG_Vr[1] /= Number_of_Samples;

    JoyStick_Error =  2 + (ABS(AVG_Vr[0] - Vr_rest_value[0]) + ABS(AVG_Vr[1] - Vr_rest_value[1]))/2;
}
//-------------------------------------------------------------
//                      Joystick Painter
//-------------------------------------------------------------
void JoyStickADC_Painter(){
    EnableJoystickInt(); // allow interrupt only in the end of cycle
    i = 0;
    if(!stateIFG) { //send data
        DisableADC();
        EnableADC(&Vr);
        EnterLPM();
        TXBuffer = Vr[i] & 0xFF;
        MSBIFG = 1;
        EnableTXIE();
        EnterLPM();
    }

    else if (stateIFG) { //send state
        TXBuffer = state_changed[i] & 0xFF;
        MSBIFG = 1;
        EnableTXIE();
        EnterLPM();
        START_TIMERA1(5000);
        ClearJoystickIFG();
    }
    EnableJoystickInt();
}


//-------------------------------------------------------------
//                       Script
//-------------------------------------------------------------
void ScriptFunc() {

    if(ExecuteFlag){
        ExecuteFlag = 0;
        flag_script = 1;
        ExecuteScript();
    }
}
//-------------------------------------------------------------
//                       Execute Script
//-------------------------------------------------------------
void ExecuteScript(void)
{
    char *Flash_ptrscript;                         
    char OPC[10], Var1_Flash[20], Var2_Flash[20];
    unsigned int Var1, Var2, y;
    if (flag_script)
        Flash_ptrscript = file.file_start_pointers[burn_index];
    flag_script = 0;
    for (y = 0; y < 64;){
        OPC[0] = *Flash_ptrscript++;
        OPC[1] = *Flash_ptrscript++;
        y = y + 2;
        switch (OPC[1]){
            case '1':
                Var1_Flash[0] = *Flash_ptrscript++;
                Var1_Flash[1] = *Flash_ptrscript++;
                y = y + 2;
                Var1 = hex2int(Var1_Flash);
                inc_lcd(Var1);
                break;

            case '2':
                Var1_Flash[0] = *Flash_ptrscript++;
                Var1_Flash[1] = *Flash_ptrscript++;
                y = y + 2;
                Var1 = hex2int(Var1_Flash);
                dec_lcd(Var1);
                break;

            case '3':
                Var1_Flash[0] = *Flash_ptrscript++;
                Var1_Flash[1] = *Flash_ptrscript++;
                y = y + 2;
                Var1 = hex2char(Var1_Flash);
                rra_lcd(Var1);
                break;

            case '4':
                Var1_Flash[0] = *Flash_ptrscript++;
                Var1_Flash[1] = *Flash_ptrscript++;
                y = y + 2;
                Var1 = hex2int(Var1_Flash);
                set_delay(Var1);
                break;

            case '5':
                clear_LCD();
                break;

            case '6': //point stepper motor to degree p
                Var1_Flash[0] = *Flash_ptrscript++;
                Var1_Flash[1] = *Flash_ptrscript++;
                y = y + 2;
                Var1 = hex2int(Var1_Flash);
                stepper_deg(Var1);
                break;

            case '7': //scan area between angle l to r
                Var1_Flash[0] = *Flash_ptrscript++;
                Var1_Flash[1] = *Flash_ptrscript++;
                y = y + 2;
                Var2_Flash[0] = *Flash_ptrscript++;
                Var2_Flash[1] = *Flash_ptrscript++;
                y = y + 2;
                Var1 = hex2int(Var1_Flash);
                Var2 = hex2int(Var2_Flash);
                stepper_scan(Var1, Var2);
                break
                ;
            case '8': // go sleep
                state = state0;
                break;
        }
    }
}
//-------------------------------------------------------------
//                Script Mode Functions 
//-------------------------------------------------------------
//-------------------------------------------------------------
//                Count up
//-------------------------------------------------------------
// Function to increment display from 0 to x with delay
void inc_lcd(int x) {
    unsigned int i;
    for (i = 0; i <= x; i++) {
        char buffer[16]; // Assuming a 16-character wide display
        sprintf(buffer, "%d", i);
        lcd_clear();
        lcd_puts(buffer); // Display the value onto the LCD
        timer_delay(delay_value * 10); // Timer-based delay
    }
}
//-------------------------------------------------------------
//                Count down
//-------------------------------------------------------------
// Function to decrement display from x to 0 with delay
void dec_lcd(int x) {
    int i;
    for (i = x; i >= 0; i--) {
        char buffer[16];
        sprintf(buffer, "%d", i);
        lcd_clear();
        lcd_puts(buffer); // Display the value onto the LCD
        timer_delay(delay_value * 10); // Timer-based delay
    }
}
//-------------------------------------------------------------
//                Rotate Right LCD
//-------------------------------------------------------------
// Function to rotate right on LCD with a single character (ASCII)
void rra_lcd(char x) {
    unsigned int i;
    unsigned int j;
    lcd_home();   // Start at the beginning of the screen
    lcd_clear();  // Clear the display

    for (i = 0; i < 32; i++) {  // Loop through all 32 positions (0 to 31)
        lcd_home();  // Reset cursor to the home position

        // Move the cursor to the desired position
        for (j = 0; j < i; j++) {
            if (j == 15) lcd_new_line;  // Move to the second line after the first 16 positions
            else lcd_cursor_right();  // Move cursor right for each step
        }
        lcd_data(x);  // Display the character at the current position
        timer_delay(delay_value * 10);  // Timer-based delay for each step
        lcd_clear();  // Clear the display before the next shift
    }
}
//-------------------------------------------------------------
//               Set Delay
//-------------------------------------------------------------
// Function to set the delay value for other operations
void set_delay(int d) {
    delay_value = d;
}
//-------------------------------------------------------------
//                Clear LCD
//-------------------------------------------------------------
// Function to clear LCD screen
void clear_LCD() {
    lcd_clear(); // Clear display command
}
//-------------------------------------------------------------
//             Move Stepper Motor to Degree
//-------------------------------------------------------------
// Function to move stepper motor to a specific degree
void stepper_deg(int p) {
    char* deg_str[4];
    sprintf(deg_str, "%d", p);
    send_to_PC(deg_str); // Send the degree to the PC
    GotoAngle((unsigned long)p * (unsigned long)SCALE_FACTOR); // Move stepper motor to the specified degree
}
//-------------------------------------------------------------
//         Scan Stepper Motor from Degree1 to Degree2
//-------------------------------------------------------------
// Function to scan between two degrees using stepper motor and display on LCD
void stepper_scan(int l, int r) {
    // gotoAngle finds the shortest path to the desired angle, GotAngleClockWise moves only in the clockwise direction
    char str_A[] = "I'm on my way!";
    char str_B[] = "Scanning...";
    char str_C[] = "Reached the first angle!";
    char str_D[] = "Reached the second angle!";
    
    lcd_clear();

    lcd_puts(str_A); // Display scanning message on LCD
    GotoAngle((unsigned long)l * (unsigned long)SCALE_FACTOR); // Move to the leftmost angle
    timer_delay(delay_value * 5); // Timer-based delay
    clear_LCD();
    lcd_puts(str_C);    // Display the reached angle on LCD
    timer_delay(delay_value * 10); // Timer-based delay
    clear_LCD();
    lcd_puts(str_B); // Display scanning message on LCD
    timer_delay(delay_value * 5); // Timer-based delay
    GotoAngleClockWise((unsigned long)r * (unsigned long)SCALE_FACTOR); // Move to the rightmost angle
    lcd_clear();
    lcd_puts(str_D);    // Display the reached angle on LCD
    timer_delay(delay_value * 10); // Timer-based delay
    clear_LCD();  
}   
//-------------------------------------------------------------
//                   Sleep Mode
//-------------------------------------------------------------
// Function to put the MCU into sleep mode
void sleep_mcu() {
    EnterLPM(); // Enter low-power mode
}
