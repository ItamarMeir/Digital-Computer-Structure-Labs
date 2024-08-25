
#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include  "../header/flash.h"     // private library - FLASH layer
#include "stdio.h"
#include "math.h"

int flag_script = 1;
int16_t Vrx = 0;
int16_t Vry = 0;
int step_index = 0;
unsigned int JoyStick_Error = 0;
int AVG_Vr[2] = {0, 0};
//-------------------------------------------------------------
//                Stepper Using JoyStick
//-------------------------------------------------------------
void StepperUsingJoyStick(){
    uint32_t counter_phi;
    uint32_t phi;
    uint32_t temp;
    unsigned int rotation_speed[] = {200, 600, 800, 1200 };
    unsigned int selected_speed = 0;

//    curr_counter = 0;
    //EnableJoystickInt(); // Enabling PB of Joystick
   // EnableTXIE();
    while (counter != 0 && state==state0 && stateStepp==stateJSRotate){
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

        // if (AVG_Vr[1] > Vr_rest_value[1] + JoyStick_Error){
        //     rotation = Clockwise;
        //     }
        // else if (AVG_Vr[1] < Vr_rest_value[1] - JoyStick_Error){rotation = CounterClockwise;}
        // else {rotation = stop;}
        // selected_speed = rotation_speed[((AVG_Vr[0] * 2) / Vr_rest_value[0])];
    
    // If the joystick is not in the rest position (considering the error)
        if (!(AVG_Vr[0]<Vr_rest_value[0] + JoyStick_Error && AVG_Vr[0]>Vr_rest_value[0] - JoyStick_Error &&
             AVG_Vr[1]<Vr_rest_value[1] + JoyStick_Error && AVG_Vr[1]>Vr_rest_value[1] - JoyStick_Error)){
           
            Vrx = Vr[1] - Vr_rest_value[1];
            Vry = Vr[0] - Vr_rest_value[0];

            phi = atan2_fp(Vry, Vrx);
            temp = phi * counter;

            if (270 < phi) {
               if (0 < Vrx && Vry < 0) {
                    counter_phi = ((counter * 7) / 4) - (temp / 360);  // ((630-phi)/360)*counter;
                    }
                else {
                    counter_phi = ((counter * 3) / 4) - (temp / 360);  // ((270-phi)/360)*counter;
                    }
                if ((int)(curr_counter - counter_phi) < 0) {
                    Stepper_clockwise(600);
                    curr_counter++;
                }
                else {
                    Stepper_counter_clockwise(600);
                    curr_counter--;
                }
        }

        if (rotation != stop){
            
            START_TIMERA0(selected_speed);
            while (JoyStickCounter < JoyStick_Stepper_Rotate)
            {
                EnterLPM(); // Sleep      
            }
        }
    }
}
}
//-------------------------------------------------------------
//                StepperUsingJoyStick
//-------------------------------------------------------------
void JoyStickADC_Steppermotor(){  // Sample the Joystick and save the values in Vr[]
        DisableADC();                     // Disable ADC
        EnableADC(Vr);                    // Start ADC
        EnterLPM();                       // LPM0, ADC10_ISR will force exit
}

//-------------------------------------------------------------
//                Stepper clockwise
//-------------------------------------------------------------
// void Stepper_clockwise(long speed_Hz){
//     int speed_clk;
//     // 1 step clockwise of stepper - 50Hz
//     //speed_clk = 131072/speed_Hz;
//     speed_clk = 873; //(2^20/8)*(1/200[Hz]) = 655
//     StepmotorPortOUT = 0x01; // out = 0001
//     START_TIMERA0(speed_clk); // (2^20/8)*(1/50[Hz]) = 2621
//     StepmotorPortOUT = 0x08; // out = 1000
//     START_TIMERA0(speed_clk); // (2^20/8)*(1/50[Hz]) = 2621
//     StepmotorPortOUT = 0x04; // out = 0100
//     START_TIMERA0(speed_clk); // (2^20/8)*(1/50[Hz]) = 2621
//     StepmotorPortOUT = 0x02; // out = 0010
// }

void Activate_Stepper(long speed_Hz, int Rot_state){
    //
    // 1 step clockwise of stepper - 50Hz
    //speed_clk = 131072/speed_Hz;
    //speed_clk = 50; //(2^20/8)*(1/200[Hz]) = 655
    rotation = Rot_state;
    step_index = 0;
    START_TIMERA0(speed_Hz);
    while (rotation == Rot_state)
        {
             EnterLPM(); // Sleep
        }           
}


void Stepper_clockwise(long speed_Hz){
    int speed_clk;
    // 1 step clockwise of stepper - 50Hz
    //speed_clk = 131072/speed_Hz;
    speed_clk = 873; //(2^20/8)*(1/200[Hz]) = 655
    START_TIMERA0(speed_clk);
   
                
}

//-------------------------------------------------------------
//                Stepper counter-clockwise
//-------------------------------------------------------------
void Stepper_counter_clockwise(long speed_Hz){
    int speed_clk;
    rotation = CounterClockwise;
    // 1 step counter-clockwise of stepper
//    speed_clk = 131072/speed_Hz;
    speed_clk = 873; //(2^20/8)*(1/200[Hz]) = 655
    START_TIMERA0(speed_clk);
    while (stateStepp == stateAutoRotate)
    {
        EnterLPM(); // Sleep
    }  
}

void GotoAngle(int angle){
    curr_angle = (int)(360/((double)(max_counter)/ (double)(curr_counter)));
    int speed_clk = 873; //(2^20/8)*(1/200[Hz]) = 655
    step_index = 0;
    counter = 0;
    if (angle > curr_angle){
        rotation = Clockwise;
        START_TIMERA0(speed_clk);
        int count_until = (int)(((double)(angle - curr_angle)) / phi);
        int i = 0;
        while (counter <= count_until)
            {
                EnterLPM(); // Sleep
            }  
    }
    else{
        rotation = CounterClockwise;
        START_TIMERA0(speed_clk);
        int count_until = (int)(((double)(curr_angle - angle)) / phi);
        while (counter <= count_until)
            {
                EnterLPM(); // Sleep
            }  
    }                
}

//-------------------------------------------------------------
//                Stepper Motor Calibration
//-------------------------------------------------------------
void calibrate(){
    //int temp = 360;
    int tmp = 360; // 360 degrees in fixed point
    fix overalldeg = int2fix((int)(360)); // 360 degrees in fixed point
    max_counter = counter;
    curr_counter = 0;
    int2str(counter_str, (int)counter);
    sprintf(counter_str, "%d", counter);
    tx_index = 0;
    phi = 360.0 / (double)(max_counter);
    UCA0TXBUF = counter_str[tx_index++];
    EnableTXIE();                      // Enable USCI_A0 TX interrupt
    EnterLPM(); // Sleep
    curr_counter = 0;
}

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

    JoyStick_Error =  (ABS(AVG_Vr[0] - Vr_rest_value[0]) + ABS(AVG_Vr[1] - Vr_rest_value[1]));
}
// //-------------------------------------------------------------
// //                Stepper Motor Calibration
// //-------------------------------------------------------------
// void calibrate(){
//     max_counter = counter;
//     curr_counter = 0;
//     int2str(counter_str, (int)counter);
//   //  sprintf(counter_str, "%d", counter);
//     phi = 360.0 / max_counter;
//     send_to_PC(counter_str);
//     EnterLPM(); // Sleep
//     curr_counter = 0;
// }
//--------------------------------------------------------------

//--------------------------------------------------------------

//-------------------------------------------------------------
//
//-------------------------------------------------------------
void JoyStickADC_Painter(){
    JoyStickIntEN &= ~BIT5; // allow interrupt only in the end of cycle
    i = 0;
    if(!stateIFG) { //send data
        ADC10CTL0 &= ~ENC;
        while (ADC10CTL1 & ADC10BUSY);               // Wait if ADC10 core is active
        ADC10SA = &Vr;                        // Data buffer start
        ADC10CTL0 |= ENC + ADC10SC; // Sampling and conversion start
        __bis_SR_register(LPM0_bits + GIE);        // LPM0, ADC10_ISR will force exit

        UCA0TXBUF = Vr[i] & 0xFF;
        MSBIFG = 1;
        IE2 |= UCA0TXIE;
        __bis_SR_register(LPM0_bits + GIE);        // LPM0, will exit when finish tx

    }

    else if (stateIFG) { //send state
        UCA0TXBUF = state_changed[i] & 0xFF;
        MSBIFG = 1;
        IE2 |= UCA0TXIE;
        __bis_SR_register(LPM0_bits + GIE);        // LPM0, will exit when finish tx


        START_TIMERA0(5000); // wait for PC to get and sync after all the debounce and interrupt delay
        JoyStickIntPend &= ~BIT5;

    }

    JoyStickIntEN |= BIT5; // allow interrupt only in the end of cycle
}



//-----------For Flash-------------------------------------
void ScriptFunc() {

    if(FlashBurnIFG){
        FlashBurnIFG=0;
        FCTL2 = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator
   //     ScriptData();
        file.file_size[file.num_of_files - 1] = strlen(file_content) - 1;
        write_Seg();
        send_finish_to_PC(); // send ACK to PC
        IE2 |= UCA0RXIE;
    }
    if(ExecuteFlag){
        ExecuteFlag=0;
        flag_script = 1;
        delay_time = 500;  // delay default time
        ExecuteScript();
        send_finish_to_PC(); // finish script
    }
    __bis_SR_register(LPM0_bits + GIE);
}

//---------------Execute Script--------------------------------
void ExecuteScript(void)
{
    char *Flash_ptrscript;                         // Segment pointer
    char OPCstr[10], Operand1Flash[20], Operand2Flash[20];
    unsigned int Oper2ToInt, X, start, stop, y;
    if (flag_script)
        Flash_ptrscript = file.file_ptr[file.num_of_files - 1];
    flag_script = 0;
  //  IE2 != ~UCA0RXIE; // Now added by mg
   // for (y = 0; y <= file.file_size[file.num_of_files - 1];)
    for (y = 0; y < 64;)
    {
        OPCstr[0] = *Flash_ptrscript++;
        OPCstr[1] = *Flash_ptrscript++;
        y = y + 2;
        switch (OPCstr[1])
        {
        case '1':
            Operand1Flash[0] = *Flash_ptrscript++;
            Operand1Flash[1] = *Flash_ptrscript++;
            y = y + 2;
//            sscanf(Oper1fFromFlash, "%2x", &Oper2ToInt);
            Oper2ToInt = hex2int(Operand1Flash);
            while (Oper2ToInt > 0)
            {
                // blinkRGB();    //blink RGB LED
                Oper2ToInt--;
            }
            break;

        case '2':
            Operand1Flash[0] = *Flash_ptrscript++;
            Operand1Flash[1] = *Flash_ptrscript++;
            y = y + 2;
      //      sscanf(Oper1fFromFlash, "%2x", &Oper2ToInt);
            Oper2ToInt = hex2int(Operand1Flash);
            while (Oper2ToInt)
            {
                // rlc_leds(rotateLEDs);           //rotate left
                Oper2ToInt--;
            }
            break;
        case '3':
            Operand1Flash[0] = *Flash_ptrscript++;
            Operand1Flash[1] = *Flash_ptrscript++;
            y = y + 2;
         //   sscanf(Oper1fFromFlash, "%2x", &Oper2ToInt);
            Oper2ToInt = hex2int(Operand1Flash);
            while (Oper2ToInt)
            {
                // rrc_leds(rotateLEDs);         //rotate right
                Oper2ToInt--;
            }
            break;
        case '4':
            Operand1Flash[0] = *Flash_ptrscript++;
            Operand1Flash[1] = *Flash_ptrscript++;
            y = y + 2;
       //     sscanf(Oper1fFromFlash, "%2x", &delay_time); //set delay
            delay_time = hex2int(Operand1Flash);
            delay_time = delay_time * 10 ; //its in unit of 10ms
            break;
        case '5':
            // ClearLEDsRGB();
            break;
        case '6': //point stepper motor to degree p
            Operand1Flash[0] = *Flash_ptrscript++;
            Operand1Flash[1] = *Flash_ptrscript++;
            y = y + 2;
        //    sscanf(Oper1fFromFlash, "%2x", &X);
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
        //    sscanf(Oper1fFromFlash, "%2x", &X);
            X = hex2int(Operand1Flash);
            start = X;
            motorGoToPosition(X, OPCstr[1]);
       //     sscanf(Oper2fFromFlash, "%2x", &X);
            X = hex2int(Operand2Flash);
            stop = X;
            motorGoToPosition(X, OPCstr[1]);

            break;
        case '8': // go sleep
//            state = state0;
            break;

        }
    }
}

//-------------------------------------------------------------
//                Script Mode Functions 
//-------------------------------------------------------------

static int delay_value = 200; // Default delay value in units of 10ms


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



// Function to move stepper motor to a specific degree
void stepper_deg(int p) {
    GotoAngle(p); // Move stepper motor to the specified degree
    char buffer[16];
    sprintf(buffer, "Angle: %d", p);
    lcd_puts(buffer); // Display the degree onto the LCD
}

// Function to scan between two degrees using stepper motor and display on LCD
void stepper_scan(int l, int r) {
    unsigned int i;
    for (i = l; i <= r; i++) {
        GotoAngle(i); // Move stepper motor incrementally between l and r
        char buffer[16];
        sprintf(buffer, "Angle: %d", i);
        lcd_puts(buffer); // Display degree on LCD screen
        timer_delay(delay_value * 10); // Timer-based delay
    }
}

// Function to put the MCU into sleep mode
void sleep_mcu() {
    EnterLPM(); // Enter low-power mode
}


