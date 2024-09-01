#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer

extern void JoyStickADC_Painter();
extern void JoyStickADC_Steppermotor();
extern void Activate_Stepper(int, int);
extern void clear_counters();
extern void change_delay_time();
extern void calibrate();
extern void ScriptFunc();
extern void ExecuteScript();
extern void inc_lcd(int);
extern void rra_lcd(char);
extern void GotoAngle(unsigned long int);
extern void JoyStickRestVr();
extern int16_t Vrx;
extern int16_t Vry;
extern int step_index;
extern int calculateJoystickAngle(int x, int y);
extern void rotateMotorUsingJoystick(int x, int y);
extern void Activate_Stepper_Clicks(int clicks, int speed_Hz, int Rot_state);
extern void stepper_scan(int l, int r);
extern char x;
extern char y;

#define ALPHA 0.1
#define Number_of_Samples 1
#define ABS(x) ((x) < 0 ? -(x) : (x))

#define CENTER_X 460          // Center position for X-axis
#define CENTER_Y 480          // Center position for Y-axis
#define DEAD_ZONE 20          // Dead zone around the center position
#define SCALE_FACTOR 1000     // Scaling factor for fixed-point calculations
#define MAX_VALUE 949         // Maximum value from the joystick readings
#define MIN_SPEED 200         // Minimum speed in Hz to activate the motor
#define MAX_SPEED 1000        // Maximum speed in Hz


#endif







