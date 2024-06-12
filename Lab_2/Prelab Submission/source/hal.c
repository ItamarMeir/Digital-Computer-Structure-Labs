#include  "../header/hal.h"     // private library - HAL layer

//--------------------------------------------------------------------
//             System Configuration
//--------------------------------------------------------------------
void sysConfig(void){
	GPIOconfig();
	TIMERconfig();
	ADCconfig();
	
}
//--------------------------------------------------------------------
// 				Print Byte to 8-bit LEDs array
//--------------------------------------------------------------------
void print2LEDs(unsigned char ch){
	LEDsArrPort = ch;
}
//--------------------------------------------------------------------
//				Clear 8-bit LEDs array
//--------------------------------------------------------------------
void clrLEDs(void){
	LEDsArrPort = 0x000;
}
//--------------------------------------------------------------------
//				Toggle 8-bit LEDs array
//--------------------------------------------------------------------
void toggleLEDs(char ch){
	LEDsArrPort ^= ch;
}
//--------------------------------------------------------------------
//				Set 8-bit LEDs array
//--------------------------------------------------------------------
void setLEDs(char ch){
	LEDsArrPort |= ch;
}
//--------------------------------------------------------------------
//				Get 8-bit LEDs array
//--------------------------------------------------------------------
char getLEDs(){
	return LEDsArrPort;
}
//--------------------------------------------------------------------
//				Read value of 4-bit SWs array
//--------------------------------------------------------------------
unsigned char readSWs(void){
	unsigned char ch;
	ch = SWsArrPort;
	ch &= SWmask;     // mask the least 4-bit
        ch = (ch >> 4) | (ch << 4);
	return ch;
}
unsigned char read_single_SW(unsigned char sw){
	return readSWs() & sw;
}
//---------------------------------------------------------------------
//             Increment / decrement LEDs shown value
//---------------------------------------------------------------------
void incLEDs(char val){
	LEDsArrPort += val;
}
//---------------------------------------------------------------------
//            Shift left LED
//---------------------------------------------------------------------
void shlLEDs(char val){
	LEDsArrPort <<= val;
}

//---------------------------------------------------------------------
//            Polling based Delay function
//---------------------------------------------------------------------
void delay(unsigned int t){  // t[msec]
	volatile unsigned int i;
	
	for(i=t; i>0; i--);
}
//---------------------------------------------------------------------
//            Set pin 7 in OutArrPortOut
//---------------------------------------------------------------------
void setOutputPin(){
	OutArrPortOut |= OUTPUT_PIN;
}
//---------------------------------------------------------------------
//            Reset pin 7 in OutArrPortOut
//---------------------------------------------------------------------
void resetOutputPin(){
	OutArrPortOut &= ~OUTPUT_PIN;
}
//---------------------------------------------------------------------
//            Enter from LPM0 mode
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
//            Port2 Interrupt Service Rotine
//*********************************************************************
#pragma vector=PORT2_VECTOR
  __interrupt void PBs_handler(void){

	delay(debounceVal);
//---------------------------------------------------------------------
//            selector of transition between states
//---------------------------------------------------------------------
	if(PBsArrIntPend & PB0){
	  state = state1;
	  PBsArrIntPend &= ~PB0;	
        }
        else if(PBsArrIntPend & PB1){
	  state = state2;
	  PBsArrIntPend &= ~PB1;
	    }
	else if(PBsArrIntPend & PB2){
	  state = state3;
	  PBsArrIntPend &= ~PB2;
        }
	// else if(PBsArrIntPend & PB3 || PBsArrIntPend == 0x88){
	// 	state = state4;
	//   	PBsArrIntPend &= ~PB3;
    //     }
	
//---------------------------------------------------------------------
//            Exit from a given LPM
//---------------------------------------------------------------------	
        switch(lpm_mode){
		case mode0:

		 LPM0_EXIT;; // must be called from ISR only
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

//*********************************************************************
//            TimerA0 Interrupt Service Routine
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{

    LPM0_EXIT;
    TACTL = MC_0+TACLR;
}


//*********************************************************************
//            TimerA1 Interrupt Service Routine
//*********************************************************************
unsigned int clk_overflows = 0;
unsigned int t_0;
unsigned int t_1;
unsigned int count = 0;


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A1_VECTOR))) TIMER1_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(TA1IV, 0x0A))
  {
    case  TA1IV_NONE: break;              // Vector  0:  No interrupt
    case  TA1IV_TACCR1:                   // Vector  2:  TACCR1 CCIFG
          TA1CTL &= ~(TAIFG);
        break;
    case TA1IV_TACCR2:                    // Vector  4:  TACCR2 CCIFG
        if (TA1CCTL2 & CCI) {               // Capture Input Pin Status
		  
		  if (count == 0) {
			t_0 = TA1CCR2;
			count = 1;
		  } else {
			t_1 = TA1CCR2;
			count = 2;
		  }
                     
                      
		}
        break;
    case TA1IV_6: break;                  // Vector  6:  Reserved CCIFG
    case TA1IV_8: break;                  // Vector  8:  Reserved CCIFG
    case TA1IV_TAIFG:						 // Vector 10:  TAIFG
		clk_overflows = 1;
	    break;             
      default:  break;
  }
}

//*********************************************************************
//            ADC10 Vector Interrupt Service Routine
//*********************************************************************
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    __bic_SR_register_on_exit(CPUOFF);
}

// --------------------------------------------------------
//              TimerA Drivers
// --------------------------------------------------------
char read_TACCR0(void){
	return TACCR0;
}
char read_TACCR1(void){
	return TACCR1;
}


//******************************************************************
//    write frequency template to LCD for state1
//******************************************************************
void write_freq_tmp_LCD(){
   lcd_clear();
   lcd_home();
	const char SquareWaveFreq[] = "fin=";
    const char Hz[] = "Hz";
     lcd_puts(SquareWaveFreq);
     lcd_cursor_right();
     lcd_cursor_right();
     lcd_cursor_right();
     lcd_cursor_right();
     lcd_cursor_right();
     lcd_puts(Hz);
}

// --------------------------------------------------------
//              LCD Driver
// --------------------------------------------------------

/* - - - - - - - LCD interface - - - - - - - - -
 *	This code will interface to a standard LCD controller
 *  It uses it in 4 or 8 bit mode.
 */

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
// initialize the LCD
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
  asm("nop");
  asm("nop");
  LCD_EN(0);
}

//---------------------------------------------------------------------
// 		  			End of LCD Driver
//---------------------------------------------------------------------