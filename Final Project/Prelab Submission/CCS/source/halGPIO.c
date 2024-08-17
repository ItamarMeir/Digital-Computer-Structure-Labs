#include  "../header/halGPIO.h"     // private library - HAL layer
#include "stdio.h"
#include<string.h>

//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
	GPIOconfig();
	TIMER0_A0_config();
    TIMER1_A0_config();
	ADCconfig();
	UARTconfig();
}
//---------------------------------------------------------------------
//            Polling based Delay function
//---------------------------------------------------------------------
void delay(unsigned int t){  // t[msec]
	volatile unsigned int i;
	
	for(i=t; i>0; i--);
}
//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
	if (LPM_level == 0x00) 
	  _BIS_SR(LPM0_bits + GIE);     /* Enter Low Power Mode 0 */
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
//            LCD
//---------------------------------------------------------------------
//******************************************************************
// send a command to the LCD
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
// send data to the LCD
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
// write a string of chars to the LCD
//******************************************************************
void lcd_puts(const char * s){

    while(*s)
        lcd_data(*s++);
}

//******************************************************************
//                        init the LCD
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
// lcd strobe functions
//******************************************************************
void lcd_strobe(){
  LCD_EN(1);
  asm("NOP");
 // asm("NOP");
  LCD_EN(0);
}
//---------------------------------------------------------------------
//            RGB functions
//---------------------------------------------------------------------
void clear_RGB(){
    RGBArrPortOut = 0x00;
}

void assign_RGB_value(unsigned int value){
    RGBArrPortOut = value;
}

//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}
 
//*********************************************************************
//            TimerA0 Interrupt Service Routine
//*********************************************************************
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR (void){
  if (Timer0_CTL & TAIFG){
    Timer0_CTL &= ~TAIFG;            //turn off flag
    LPM0_EXIT;
  }
}

//-------------------------------------------------------------
//              StartTimer and FinishTimer For Count UpDown
//-------------------------------------------------------------
void startTimerA0(){
    Timer0_CTL |= TASSEL_2 + MC_3 + ID_3 + TACLR; //  select: 2 - SMCLK ; control: 3 - Up/Down  ; divider: 3 - /8
    Timer0_CTL |= TAIE;
}

void finishTimerA0(){
    Timer0_CTL |= TASSEL_2 + MC_0 + ID_3 + TACLR;
    Timer0_CTL &= ~TAIE;
}

//-------------------------------------------------------------
//   start/shutdown timer and enable/disable interrupts
//-------------------------------------------------------------
void startTimerA1(){
    Timer1_CTL |= TASSEL_2 + MC_1 + ID_0 + TACLR; //  select: 2 - SMCLK ; control: 1 - Up  ; divider: 0 - /1   
}

void finishTimerA1(){
    Timer1_CTL |= TASSEL_2 + MC_0 + ID_0 + TACLR;
}

void enableTimerA1_bzr(){
    Timer1_CCTL2 |=  OUTMOD_7;  // 7 - reset/set
}

void disableTimerA1_bzr(){
    Timer1_CCTL2 &=  ~OUTMOD_7;
}
//*********************************************************************
//            ADC10 Vector Interrupt Service Routine
//*********************************************************************
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    LPM0_EXIT;
}
//-------------------------------------------------------------
//   start/shutdown ADC
//-------------------------------------------------------------
void startADC10(){
     ADC_CTL0 |= ENC + ADC10SC;
}

void finishADC10(){
    ADC_CTL0 &= ~ADC10ON;
}

int get_ADC_value(){
    return ADC_MEM;
}
//-------------------------------------------------------------
//                      UART functions
//-------------------------------------------------------------
void enable_UARTRX_interrupts(){
    IE2 |= UCA0RXIE;                  // Enable USCI_A0 RX interrupt
}


//-------------------------------------------------------------
//                      UART TX ISR - Recieve from PC
//-------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if (X_flag){                // If X_flag is greater then 0, then we are in the process of setting X
        if (X_flag < 6){    // If X_flag is less then 6, then we are still in the process of setting X
            UCA0TXBUF = '4'; // Send next Char
        }
        else{   
            UCA0TXBUF = 'E'; // Error - Char sent is not 0-9.
            X_flag = 0;
        }
    }
    else if (state == state7){
        UCA0TXBUF = '7'; // Print Menu
    }
    else{
        UCA0TXBUF = 'R'; // Received
    }
    IE2 &= ~UCA0TXIE;
}

//-------------------------------------------------------------
//                      UART RX ISR - Send to PC
//-------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  if (UCA0RXBUF == '1' && !X_flag){     // If 1 is pressed, blink RGB
      state = state1;
      IE2 |= UCA0TXIE;
  }

  else if (UCA0RXBUF == '2' && !X_flag){ // If 2 is pressed, count up to LCD
      state = state2;
      IE2 |= UCA0TXIE;
  }

  else if (UCA0RXBUF == '3' && !X_flag){ // If 3 is pressed, shift buzzer frequency
      state = state3;
      IE2 |= UCA0TXIE;
  }

  else if (UCA0RXBUF == '4' || X_flag){ // If 4 is pressed, set X
      if(!X_flag){          // If X_flag is 0, then we are starting to set X
          X_flag = 1;       // Set X_flag to 1
          IE2 |= UCA0TXIE;  // Enable USCI_A0 TX interrupt
          state = state8;   // Set state to sleep mode
      }
      else{
          if(UCA0RXBUF == '\n'){    // If Enter is pressed, then we are done setting X
              timerInput[X_flag - 1] = UCA0RXBUF; // Set the last char to '\n'
              timerInput[X_flag - 1] = '\0';    // Set the next char to '\0'
              X_flag = 0;   // Reset X_flag
              state = state4;   // Set state to set X
          }
          else if (UCA0RXBUF >= '0' && UCA0RXBUF <= '9') { // If the char is between 0-9, then we are still setting X
              timerInput[X_flag - 1] = UCA0RXBUF;   // Set the char to the timerInput array
              X_flag++;                             // Increment X_flag
          }
          else{
              X_flag = 404;  // Error - Char sent is not 0-9.
              state = state8; // Set state to sleep mode
          }
      }
      IE2 |= UCA0TXIE;
  }

  else if (UCA0RXBUF == '5' && !X_flag){        // If 5 is pressed, measure POT
      state = state5;
      IE2 |= UCA0TXIE;
  }

  else if (UCA0RXBUF == '6' && !X_flag){       // If 6 is pressed, reset num value and clear LCD
      state = state6;
      IE2 |= UCA0TXIE;
  }

  else if (UCA0RXBUF == '7' && !X_flag){      // If 7 is pressed, Menu display on PC side
      state = state7;
      IE2 |= UCA0TXIE;
  }

  else if (UCA0RXBUF == '8' && !X_flag){    // If 8 is pressed, then we are at sleep mode
      state = state8;
      IE2 |= UCA0TXIE;
  }

  else {
      X_flag = 404;         // Error - Char sent is not 0-9.
      state = state8;    // Set state to sleep mode
      IE2 |= UCA0TXIE;
  }

  switch(lpm_mode){
      case mode0:
       LPM0_EXIT; // must be called from ISR only
       break;

      case mode1:
       LPM1_EXIT; // must be called from ISR only
       break;

      case mode2:
       LPM2_EXIT; // must be called from ISR only
       break;

      case mode3:
       LPM3_EXIT; // must be called from ISR only
       break;

      case mode4:
       LPM4_EXIT; // must be called from ISR only
       break;
    }
}






