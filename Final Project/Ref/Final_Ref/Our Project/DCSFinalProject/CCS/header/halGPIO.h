#ifndef _halGPIO_H_
#define _halGPIO_H_

#include  "../header/bsp.h"    		// private library - BSP layer
#include  "../header/app.h"    		// private library - APP layer
#include "stdint.h"

#define MULTIPLY_FP_RESOLUTION_BITS 15 // For atan2-fixed point

typedef signed int fix;
#define multfix(a,b) ((fix)(((signed long long)(a) * (signed long long)(b)) >> 15))
#define divfix(a,b) ((fix)(((signed long long)(a) << 15) / (b)))
#define int2fix(a) ((fix)((a) << 15))
#define fix2int(a) ((int)((a) >> 15))
#define float2fix(a) ((fix)((a) * 32768.0))
#define fix2float(a) ((float)(a) / 32768.0)
#define sqrtfix(a) (float2fix(sqrt(fix2float(a))))


extern enum FSMstate state;   // global variable
extern enum Stepperstate stateStepp;
extern enum SYSmode lpm_mode; // global variable
extern enum RotationState rotation;

extern unsigned int delay_time;
extern int startRotateLEDs;
extern int* rotateLEDs;
extern int pushedIFG;
extern int sendIFG;
extern short MSBIFG;
extern short stateIFG;
extern const short state_changed[];
extern char stringFromPC[];
extern char file_content[];
extern int ExecuteFlag;
extern int FlashBurnIFG;
extern int SendFlag;
extern int j;
extern short Vr[];
extern int counter;
extern int rotateIFG;
extern char counter_str[];
extern unsigned int i;
extern unsigned int tx_index;
#define clk_tmp 131            // (2^20) / (8 * 10^ 3) to convert ms to counter value for TACCR0
#define max_timer_count 65535  // Maximum value the timer can handle (16-bit)
extern int curr_counter;
extern int max_counter;
extern int step_index;
extern int curr_angle;
extern double phi;
extern short Vr_rest_value[];

extern unsigned int JoyStickCounter;
#define JoyStick_Stepper_Rotate 60
extern void EnableADC(short* DataBufferStart);
extern void sysConfig(void);
// extern void print2RGB(char);
// extern void print2LEDs(unsigned char);
// extern void SetByteToPort(char);
// extern void clrPortByte(char);
extern void delay(unsigned int);
extern void enterLPM(unsigned char);
extern void enable_interrupts();

extern void disable_interrupts();
extern void timer_call_counter();
extern void timer_delay();
extern void START_TIMERA0(unsigned int counter);
extern void START_TIMERA1(unsigned int counter);
extern float angle(float X, float Y);
extern void  motorGoToPosition(uint32_t stepper_degrees, char script_state);
extern int16_t atan2_fp(int16_t y_fp, int16_t x_fp);
extern int16_t fixed_mul(int16_t a, int16_t b);
extern int16_t fixed_div(int16_t a, int16_t b);
extern long fixed_mul_u(long a, long b);
extern long fixed_div_u(long a, long b);
extern void send_finish_to_PC();
extern void send_to_PC(const char *str);
extern uint32_t hex2int(char *hex);

extern __interrupt void Timer_A0(void);
extern __interrupt void Timer_A1(void);
extern __interrupt void Timer1_A0_ISR(void);
extern __interrupt void USCI0RX_ISR(void);
extern __interrupt void USCI0TX_ISR(void);
extern __interrupt void Joystick_handler(void);

#endif

// #define CHECKBUSY    1  // using this define, only if we want to read from LCD

#ifdef CHECKBUSY
    #define LCD_WAIT lcd_check_busy()
#else
    #define LCD_WAIT DelayMs(5)
#endif

/*----------------------------------------------------------
  CONFIG: change values according to your port pin selection
------------------------------------------------------------*/
#define LCD_EN(a)   (!a ? (LCD_CTL_OUT&=~LCD_EN_PIN) : (LCD_CTL_OUT|=LCD_EN_PIN)) // P2.5 is lcd enable pin
#define LCD_EN_DIR(a)   (!a ? (LCD_CTL_DIR&=~LCD_EN_PIN) : (LCD_CTL_DIR|=LCD_EN_PIN)) // P2.5 pin direction

#define LCD_RS(a)   (!a ? (LCD_CTL_OUT&=~LCD_RS_PIN) : (LCD_CTL_OUT|=LCD_RS_PIN)) // P2.6 is lcd RS pin
#define LCD_RS_DIR(a)   (!a ? (LCD_CTL_DIR&=~LCD_RS_PIN) : (LCD_CTL_DIR|=LCD_RS_PIN)) // P2.6 pin direction

#define LCD_RW(a)   (!a ? (LCD_CTL_OUT&=~LCD_RW_PIN) : (LCD_CTL_OUT|=LCD_RW_PIN)) // P2.7 is lcd RW pin
#define LCD_RW_DIR(a)   (!a ? (LCD_CTL_DIR&=~LCD_RW_PIN) : (LCD_CTL_DIR|=LCD_RW_PIN)) // P2.7 pin direction

#define LCD_DATA_OFFSET 0x04 //data pin selection offset for 4 bit mode, variable range is 0-4, default 0 - Px.0-3, no offset


/*---------------------------------------------------------
  END CONFIG
-----------------------------------------------------------*/
#define FOURBIT_MODE    0x0
#define EIGHTBIT_MODE   0x1
#define LCD_MODE        FOURBIT_MODE

#define OUTPUT_PIN      1
#define INPUT_PIN       0
#define OUTPUT_DATA     (LCD_MODE ? 0xFF : (0x0F << LCD_DATA_OFFSET))
#define INPUT_DATA      0x00

#define LCD_STROBE_READ(value)  LCD_EN(1), \
                asm("nop"), asm("nop"), \
                value=LCD_DATA_READ, \
                LCD_EN(0)

#define lcd_cursor(x)       lcd_cmd(((x)&0x7F)|0x80)
#define lcd_clear()         lcd_cmd(0x01)
#define lcd_putchar(x)      lcd_data(x)
#define lcd_goto(x)         lcd_cmd(0x80+(x))
#define lcd_cursor_right()  lcd_cmd(0x14)
#define lcd_cursor_left()   lcd_cmd(0x10)
#define lcd_display_shift() lcd_cmd(0x1C)
#define lcd_home()          lcd_cmd(0x02)
#define cursor_off          lcd_cmd(0x0C)
#define cursor_on           lcd_cmd(0x0F)
#define lcd_function_set    lcd_cmd(0x3C) // 8bit,two lines,5x10 dots
#define lcd_new_line        lcd_cmd(0xC0)

extern void lcd_cmd(unsigned char);
extern void lcd_data(unsigned char);
extern void lcd_puts(const char * s);
extern void lcd_init();
extern void lcd_strobe();
extern void DelayMs(unsigned int);
extern void DelayUs(unsigned int);
