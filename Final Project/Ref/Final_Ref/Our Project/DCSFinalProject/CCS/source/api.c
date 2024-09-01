
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
static int delay_value = 200; // Default delay value in units of 10ms


// //-------------------------------------------------------------
// //                Stepper Using JoyStick
// //-------------------------------------------------------------
// void StepperUsingJoyStick(){
//     unsigned int rotation_speed[] = {200, 600, 800, 1200 };
//     unsigned int selected_speed = 0;
//     enum RotationState rotation = stop;

//     while (counter != 0 && state == state0 && stateStepp == stateJSRotate) {
//         // Sample the joystick inputs
//         SampleJoystick();

//         // Adjust Vrx and Vry relative to the center
//         int Vrx = Vr[1] - CENTER_X;
//         int Vry = Vr[0] - CENTER_Y;

//         // Select rotation speed based on Vry (Y-axis value)
//         if (Vry >= -DEAD_ZONE && Vry <= DEAD_ZONE) {
//             selected_speed = 0;  // No movement (within dead zone)
//         } else if (Vry > DEAD_ZONE && Vry <= (CENTER_Y + (MAX_VALUE - CENTER_Y) / 4)) {
//             selected_speed = rotation_speed[0];  // 200 Hz
//         } else if (Vry > (CENTER_Y + (MAX_VALUE - CENTER_Y) / 4) && Vry <= (CENTER_Y + (MAX_VALUE - CENTER_Y) / 2)) {
//             selected_speed = rotation_speed[1];  // 600 Hz
//         } else if (Vry > (CENTER_Y + (MAX_VALUE - CENTER_Y) / 2) && Vry <= (CENTER_Y + (MAX_VALUE - CENTER_Y) * 3 / 4)) {
//             selected_speed = rotation_speed[2];  // 800 Hz
//         } else {
//             selected_speed = rotation_speed[3];  // 1200 Hz
//         }

//         // Determine rotation direction based on Vrx (X-axis value)
//         if (Vrx >= -DEAD_ZONE && Vrx <= DEAD_ZONE) {
//             rotation = stop;  // No rotation (within dead zone)
//         } else if (Vrx > DEAD_ZONE) {
//             rotation = Clockwise;  // Clockwise rotation
//         } else if (Vrx < -DEAD_ZONE) {
//             rotation = CounterClockwise;  // Counterclockwise rotation
//         }

//         // Only activate the motor if the speed is greater than 200 Hz and the rotation is valid
//         if (selected_speed >= 200 && rotation != stop) {
//             Activate_Stepper(selected_speed, rotation);
//         } else {
//             Activate_Stepper(0, stop);  // Stop the motor if conditions are not met
//         }
//     }
// }
      
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
//               Joy Stick Angle - Optional
//-------------------------------------------------------------
// int calculateJoystickAngle(int x, int y) {
//     // Adjust the X and Y values relative to the joystick center
//     x -= Vr_rest_value[1]; //CENTER_X;
//     y -= Vr_rest_value[0]; //CENTER_Y;

//     // Handle cases where the joystick is within the neutral dead zone
//     if (x >= -DEAD_ZONE && x <= DEAD_ZONE) {
//         if (y > DEAD_ZONE) return 0;   // Joystick pushed straight up
//         if (y < -DEAD_ZONE) return 180; // Joystick pushed straight down
//         return 0;                       // Joystick is near the center
//     }

//     if (y >= -DEAD_ZONE && y <= DEAD_ZONE) {
//         if (x > DEAD_ZONE) return 90;    // Joystick pushed straight right
//         if (x < -DEAD_ZONE) return 270; // Joystick pushed straight left
//     }

//     // Scale the adjusted X and Y values for precision
//     // unsigned long scaled_x = ((unsigned long)x * (unsigned long)SCALE_FACTOR) / (unsigned long)(MAX_VALUE - (Vr_rest_value[1] - DEAD_ZONE));
//     // unsigned long scaled_y = ((unsigned long)y * (unsigned long)SCALE_FACTOR) / (unsigned long)(MAX_VALUE - (Vr_rest_value[0] - DEAD_ZONE));
//     // long scaled_x = ((long)x * (long)SCALE_FACTOR) / (unsigned long)Vr_rest_value[1];
//     // long scaled_y = ((long)y * (long)SCALE_FACTOR) / (unsigned long)Vr_rest_value[0];

//     // // Calculate an approximate angle using integer math
//     // unsigned long angle = (scaled_y * (unsigned long)100) / scaled_x;  // Approximation of tan(y/x) * 100

//     // // Determine the correct quadrant and adjust the angle accordingly
//     // unsigned long degrees;
//     // if (scaled_x > 0 && scaled_y >= 0) {
//     //     degrees = (angle * (unsigned long)57) / 100;              // First quadrant
//     // } else if (scaled_x > 0 && scaled_y < 0) {
//     //     degrees = (unsigned long)360 + (angle * (unsigned long)57) / 100;        // Fourth quadrant
//     // } else if (scaled_x < 0 && scaled_y >= 0) {
//     //     degrees = (unsigned long)180 - (angle * (unsigned long)57) / 100;        // Second quadrant
//     // } else {
//     //     degrees = (unsigned long)180 + (angle * (unsigned long)57) / 100;        // Third quadrant
//     // }



// // int calculateJoystickAngle(int x, int y) {
// //     // Adjust the X and Y values relative to the joystick center
// //     x -= CENTER_X;
// //     y -= CENTER_Y;

// //     // Handle cases where the joystick is within the neutral dead zone
// //     if (x >= -DEAD_ZONE && x <= DEAD_ZONE) {
// //         if (y > DEAD_ZONE) return 90;   // Joystick pushed straight up
// //         if (y < -DEAD_ZONE) return 270; // Joystick pushed straight down
// //         return 0;                       // Joystick is near the center
// //     }

// //     if (y >= -DEAD_ZONE && y <= DEAD_ZONE) {
// //         if (x > DEAD_ZONE) return 0;    // Joystick pushed straight right
// //         if (x < -DEAD_ZONE) return 180; // Joystick pushed straight left
// //     }

// //     // Scale the adjusted X and Y values for precision
// //     int scaled_x = (x * SCALE_FACTOR) / (MAX_VALUE - (CENTER_X - DEAD_ZONE));
// //     int scaled_y = (y * SCALE_FACTOR) / (MAX_VALUE - (CENTER_Y - DEAD_ZONE));

// //     // Calculate an approximate angle using integer math
// //     int angle = (scaled_y * 100) / scaled_x;  // Approximation of tan(y/x) * 100

// //     // Determine the correct quadrant and adjust the angle accordingly
// //     int degrees;
// //     if (scaled_x > 0 && scaled_y >= 0) {
// //         degrees = (angle * 57) / 100;              // First quadrant
// //     } else if (scaled_x > 0 && scaled_y < 0) {
// //         degrees = 360 + (angle * 57) / 100;        // Fourth quadrant
// //     } else if (scaled_x < 0 && scaled_y >= 0) {
// //         degrees = 180 - (angle * 57) / 100;        // Second quadrant
// //     } else {
// //         degrees = 180 + (angle * 57) / 100;        // Third quadrant
// //     }

// //     // Ensure the angle is within the range of 0 to 360 degrees
// //     return degrees % 360;
// // }
//
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
//               Rotate Motor Clockwise
//-------------------------------------------------------------
void Stepper_clockwise(int speed_Hz){
    Activate_Stepper(speed_Hz, Clockwise);
}
//-------------------------------------------------------------
//               Rotate Motor CounterClockwise
//-------------------------------------------------------------
void Stepper_counter_clockwise(int speed_Hz){
    Activate_Stepper(speed_Hz, CounterClockwise);
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

    //if (diff < ((unsigned long)(100))) return; // Ignore small angle changes

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
// GotoAngleClockWise: Rotate stepper motor to a specific angle in Clockwise direction
// Input: scaled angle in degrees (0 to 360000).
// Output: rotate stepper motor to the specified angle in Clockwise direction
//-------------------------------------------------------------
// void GotoAngleClockWise(unsigned long angle){ // assume input angle is scaled by SCALE_FACTOR and positive
//     unsigned long phi_current = ((unsigned long)curr_counter * (unsigned long)delta_phi) % (unsigned long)360000; // Current angle in scaled degrees
//     unsigned long mod_angle = angle % 360000;   // Wanted angle in scaled degrees
//     if (mod_angle == 0 && phi_current > 180000) mod_angle = 360000; // Special case for 0 degrees 
//     unsigned long diff = 0; // difference between the current angle and the wanted angle
    
//     if (mod_angle > phi_current){       // if the wanted angle is bigger than the current angle       
//         diff = mod_angle - phi_current;     
//     }
//     else{                            // if the wanted angle is smaller than the current angle
//         diff = (unsigned long)360000 - (mod_angle - phi_current);
//     }
//     unsigned int clicks_needed = (diff / (unsigned long)delta_phi) % max_counter;   // Number of clicks needed to reach the wanted angle
//     Activate_Stepper_Clicks(clicks_needed, 500, Clockwise);
// }
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


// //-------------------------------------------------------------
// // Activate_Stepper_Clicks: Rotate stepper motor a specific number of clicks
// // Input: number of clicks, speed in Hz, rotation state.
// // Output: rotate stepper motor according to the input
// //-------------------------------------------------------------
// void Activate_Stepper_Clicks(int clicks, int speed_Hz, int Rot_state){  
//     if (clicks <= 0) return;
//     rotation = Rot_state;
//     int iter = 0;
//     START_TIMERA0(speed_Hz);
//     for (iter = 0; iter < clicks; iter++){
//         EnterLPM(); // Sleep
//     }
//     rotation = stop;
//     StopTimerA0();
// }

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
//
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
        //JoyStickIntPend &= ~BIT5;

    }
    EnableJoystickInt();
    //JoyStickIntEN |= BIT5; // allow interrupt only in the end of cycle
}



// void init_filesystem() {
//     fs.file_count = 0;
//     for (int i = 0; i < MAX_FILES; i++) {
//         fs.file_start_pointers[i] = NULL;
//         fs.file_sizes[i] = 0;
//     }
// }

// void receive_script(char* file_name, uint8_t* data, uint32_t size) {
//     if (fs.file_count >= MAX_FILES) {
//         return; // No space for additional files
//     }

//     // Store file metadata
//     strcpy(fs.file_names[fs.file_count], file_name);
//     fs.file_sizes[fs.file_count] = size;
//     fs.file_start_pointers[fs.file_count] = flash_store_file(data, size); // Store file in flash memory

//     fs.file_count++;
//     send_acknowledgment(); // Send ACK to the PC
// }


// void execute_script(uint8_t script_index) {
//     if (script_index >= fs.file_count) {
//         return; // Invalid script index
//     }

//     uint32_t* script_start = fs.file_start_pointers[script_index];
//     uint32_t script_size = fs.file_sizes[script_index];

//     // Execute the script (implementation depends on the script format and commands)
//     execute_commands(script_start, script_size);
// }

// void execute_commands(uint32_t* script, uint32_t size) {
//     for (uint32_t i = 0; i < size; i++) {
//         switch (script[i]) {
//             case 'A':
//                 // Execute command A
//                 break;
//             case 'B':
//                 // Execute command B
//                 break;
//             // Add cases for other commands
//             default:
//                 break;
//         }
//     }
// }





//-----------For Flash New-------------------------------------
void ScriptFunc() {

    if(FlashBurnIFG){
        FlashBurnIFG=0;
        configureFlashTiming();
        file.file_sizes[file.num_of_files - 1] = strlen(file_content) - 1;
        write_Seg();
        finishIFG = 1;
        send_to_PC("ACK");
        finishIFG = 0;
        EnableRXIE();
    }
    if(ExecuteFlag){
        ExecuteFlag = 0;
        flag_script = 1;
        delay_time = 500;  // delay default time
        ExecuteScript();
        finishIFG = 1;
        send_to_PC("FIN");
        finishIFG = 0;
    }
    EnterLPM();
}
//---------------Execute Script New--------------------------------
void ExecuteScript(void)
{
    char *Flash_ptrscript;                         
    char OPCstr[10], Operand1Flash[20], Operand2Flash[20];
    unsigned int Oper2ToInt, X, start, stop, y;
    if (flag_script)
        Flash_ptrscript = file.file_start_pointers[file.num_of_files - 1];
    flag_script = 0;
    for (y = 0; y < 64;){
        OPCstr[0] = *Flash_ptrscript++;
        OPCstr[1] = *Flash_ptrscript++;
        y = y + 2;
        switch (OPCstr[1]){
            case '1':
                Operand1Flash[0] = *Flash_ptrscript++;
                Operand1Flash[1] = *Flash_ptrscript++;
                y = y + 2;
                Oper2ToInt = hex2int(Operand1Flash);
                inc_lcd(Oper2ToInt);
                break;

            case '2':
                Operand1Flash[0] = *Flash_ptrscript++;
                Operand1Flash[1] = *Flash_ptrscript++;
                y = y + 2;
                Oper2ToInt = hex2int(Operand1Flash);
                dec_lcd(Oper2ToInt);
                break;

            case '3':
                Operand1Flash[0] = *Flash_ptrscript++;
                Operand1Flash[1] = *Flash_ptrscript++;
                y = y + 2;
                Oper2ToInt = hex2int(Operand1Flash);
                rra_lcd(Oper2ToInt);
                break;

            case '4':
                Operand1Flash[0] = *Flash_ptrscript++;
                Operand1Flash[1] = *Flash_ptrscript++;
                y = y + 2;
                Oper2ToInt = hex2int(Operand1Flash);
                set_delay(Oper2ToInt);
                break;

            case '5':
                clear_LCD();
                break;

            case '6': //point stepper motor to degree p
                Operand1Flash[0] = *Flash_ptrscript++;
                Operand1Flash[1] = *Flash_ptrscript++;
                y = y + 2;
                X = hex2int(Operand1Flash);
                motorGoToPosition(X, OPCstr[1]);
                break;

            case '7': //scan area between angle l to r
                Operand1Flash[0] = *Flash_ptrscript++;
                Operand1Flash[1] = *Flash_ptrscript++;
                y = y + 2;
                Operand2Flash[0] = *Flash_ptrscript++;
                Operand2Flash[1] = *Flash_ptrscript++;
                y = y + 2;
                X = hex2int(Operand1Flash);
                start = X;
                motorGoToPosition(X, OPCstr[1]);
                X = hex2int(Operand2Flash);
                stop = X;
                motorGoToPosition(X, OPCstr[1]);
                break
                ;
            case '8': // go sleep
                state = state0;
                break;
        }
    }
}

//-------------------------------------------------------------
//                     Flash Script
//-------------------------------------------------------------
// void ScriptFunc() {

//     if(FlashBurnIFG){
//         FlashBurnIFG=0;
//         FCTL2 = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator
//    //     ScriptData();
//         file.file_size[file.num_of_files - 1] = strlen(file_content) - 1;
//         write_Seg();
//         send_finish_to_PC(); // send ACK to PC
//         IE2 |= UCA0RXIE;
//     }
//     if(ExecuteFlag){
//         ExecuteFlag=0;
//         flag_script = 1;
//         delay_time = 500;  // delay default time
//         ExecuteScript();
//         send_finish_to_PC(); // finish script
//     }
//     __bis_SR_register(LPM0_bits + GIE);
// }
//-------------------------------------------------------------
//                     Execute Script
//-------------------------------------------------------------
// void ExecuteScript(void)
// {
//     char *Flash_ptrscript;                         // Segment pointer
//     char OPCstr[10], Operand1Flash[20], Operand2Flash[20];
//     unsigned int Oper2ToInt, X, start, stop, y;
//     if (flag_script)
//         Flash_ptrscript = file.file_ptr[file.num_of_files - 1];
//     flag_script = 0;
//   //  IE2 != ~UCA0RXIE; // Now added by mg
//    // for (y = 0; y <= file.file_size[file.num_of_files - 1];)
//     for (y = 0; y < 64;)
//     {
//         OPCstr[0] = *Flash_ptrscript++;
//         OPCstr[1] = *Flash_ptrscript++;
//         y = y + 2;
//         switch (OPCstr[1])
//         {
//         case '1':
//             Operand1Flash[0] = *Flash_ptrscript++;
//             Operand1Flash[1] = *Flash_ptrscript++;
//             y = y + 2;
// //            sscanf(Oper1fFromFlash, "%2x", &Oper2ToInt);
//             Oper2ToInt = hex2int(Operand1Flash);
//             while (Oper2ToInt > 0)
//             {
//                 // blinkRGB();    //blink RGB LED
//                 Oper2ToInt--;
//             }
//             break;

//         case '2':
//             Operand1Flash[0] = *Flash_ptrscript++;
//             Operand1Flash[1] = *Flash_ptrscript++;
//             y = y + 2;
//       //      sscanf(Oper1fFromFlash, "%2x", &Oper2ToInt);
//             Oper2ToInt = hex2int(Operand1Flash);
//             while (Oper2ToInt)
//             {
//                 // rlc_leds(rotateLEDs);           //rotate left
//                 Oper2ToInt--;
//             }
//             break;
//         case '3':
//             Operand1Flash[0] = *Flash_ptrscript++;
//             Operand1Flash[1] = *Flash_ptrscript++;
//             y = y + 2;
//          //   sscanf(Oper1fFromFlash, "%2x", &Oper2ToInt);
//             Oper2ToInt = hex2int(Operand1Flash);
//             while (Oper2ToInt)
//             {
//                 // rrc_leds(rotateLEDs);         //rotate right
//                 Oper2ToInt--;
//             }
//             break;
//         case '4':
//             Operand1Flash[0] = *Flash_ptrscript++;
//             Operand1Flash[1] = *Flash_ptrscript++;
//             y = y + 2;
//        //     sscanf(Oper1fFromFlash, "%2x", &delay_time); //set delay
//             delay_time = hex2int(Operand1Flash);
//             delay_time = delay_time * 10 ; //its in unit of 10ms
//             break;
//         case '5':
//             // ClearLEDsRGB();
//             break;
//         case '6': //point stepper motor to degree p
//             Operand1Flash[0] = *Flash_ptrscript++;
//             Operand1Flash[1] = *Flash_ptrscript++;
//             y = y + 2;
//         //    sscanf(Oper1fFromFlash, "%2x", &X);
//             X = hex2int(Operand1Flash);
//             motorGoToPosition(X, OPCstr[1]);

//             break;
//         case '7': //scan area between angle l to r
//             Operand1Flash[0] = *Flash_ptrscript++;
//             Operand1Flash[1] = *Flash_ptrscript++;
//             y = y + 2;
//             Operand2Flash[0] = *Flash_ptrscript++;
//             Operand2Flash[1] = *Flash_ptrscript++;
//             y = y + 2;
//         //    sscanf(Oper1fFromFlash, "%2x", &X);
//             X = hex2int(Operand1Flash);
//             start = X;
//             motorGoToPosition(X, OPCstr[1]);
//        //     sscanf(Oper2fFromFlash, "%2x", &X);
//             X = hex2int(Operand2Flash);
//             stop = X;
//             motorGoToPosition(X, OPCstr[1]);

//             break;
//         case '8': // go sleep
// //            state = state0;
//             break;

//         }
//     }
// }

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
    unsigned int i;
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
// Function to clear all LEDs (or LCD screen)
void clear_LCD() {
    lcd_clear(); // Clear display command
}
//-------------------------------------------------------------
//             Move Stepper Motor to Degree
//-------------------------------------------------------------
// Function to move stepper motor to a specific degree
void stepper_deg(int p) {
    GotoAngle((unsigned long)p * (unsigned long)SCALE_FACTOR); // Move stepper motor to the specified degree
    char buffer[16];
    sprintf(buffer, "Angle: %d", p);
    lcd_puts(buffer); // Display the degree onto the LCD
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
    
    // sprintf(str_C, "Reached: %d [deg]", l);
    // char str_D[10];
    // sprintf(str_D, "Reached: %d [deg]", r);
    lcd_clear();


    lcd_puts(str_A); // Display scanning message on LCD
    GotoAngle((unsigned long)l * (unsigned long)SCALE_FACTOR); // Move to the leftmost angle
    timer_delay(delay_value * 5); // Timer-based delay
    clear_LCD();
    lcd_puts(str_C);    // Display the reached angle on LCD
    timer_delay(delay_value * 20); // Timer-based delay
    clear_LCD();
    lcd_puts(str_B); // Display scanning message on LCD
    timer_delay(delay_value * 5); // Timer-based delay
    GotoAngleClockWise((unsigned long)r * (unsigned long)SCALE_FACTOR); // Move to the rightmost angle
    lcd_clear();
    lcd_puts(str_D);    // Display the reached angle on LCD
    timer_delay(delay_value * 20); // Timer-based delay
    clear_LCD();
    
    
}   

// Function to put the MCU into sleep mode
void sleep_mcu() {
    EnterLPM(); // Enter low-power mode
}
