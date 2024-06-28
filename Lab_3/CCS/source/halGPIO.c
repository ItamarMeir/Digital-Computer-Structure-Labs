#include  "../header/halGPIO.h"     // private library - HAL layer
#include "stdio.h"
#include<string.h>

//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
	GPIOconfig();
	TIMER0_A0_config();
	DMA_config();
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
    int i = 0;
    while(*s){
        lcd_data(*s++);
        i++;
        if(i == 16 || i == 48){
            lcd_new_line;
        }
        if(i == 32 || i == 64){
            KeypadIRQIntEn |= BIT1;
            enterLPM(mode0);
            KeypadIRQIntEn &= ~BIT1;
            if(Key == 14){
                lcd_clear();
            }
        }
    }
}

//******************************************************************
// Initialise the LCD
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
void clear_LEDs(){
    LEDsArrPort = 0x00;
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
//            Port1 Interrupt Service Routine
//*********************************************************************
#pragma vector=PORT1_VECTOR
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
	else if(PBsArrIntPend & PB3){
	      state = state4;
	      PBsArrIntPend &= ~PB3;
	        }

//---------------------------------------------------------------------
//            Exit from a given LPM 
//---------------------------------------------------------------------	
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
 // *********************************************************************
//            Keypad functions
//*********************************************************************
void en_keypad_interrupts(){
    KeypadIRQIntPend &= ~BIT1;  // Clear key interrupt pending flag
    KeypadIRQIntEn |= BIT1;    // Enable key interrupts
}

void disable_keypad_interrupts(){
    KeypadIRQIntEn &= ~BIT1;    // Disable key interrupts
}

  //*********************************************************************
  //            Port2 Interrupt Service Routine
  //*********************************************************************
  #pragma vector=PORT2_VECTOR
    __interrupt void PBs_handler2(void){

      delay(debounceVal);

  //---------------------------------------------------------------------
  //            selector of Keyboard key pressed
  //---------------------------------------------------------------------
      if(KeypadIRQIntPend & BIT1){    // if keypad has been pressed find value

          Key = 16;
          KeypadPortOUT = 0x0E;
          if ( ( KeypadPortIN & 0x10 ) == 0 )  Key = 15;
          else if ( ( KeypadPortIN & 0x20 ) == 0 )  Key = 14;
          else if ( ( KeypadPortIN & 0x40 ) == 0 )  Key = 13;
          else if ( ( KeypadPortIN & 0x80 ) == 0 )  Key = 12;

          KeypadPortOUT = 0x0D;
          if ( ( KeypadPortIN & 0x10 ) == 0 )  Key = 11;
          else if ( ( KeypadPortIN & 0x20 ) == 0 )  Key = 10;
          else if ( ( KeypadPortIN & 0x40 ) == 0 )  Key = 9;
          else if ( ( KeypadPortIN & 0x80 ) == 0 )  Key = 8;

          KeypadPortOUT = 0x0B;
          if ( ( KeypadPortIN & 0x10 ) == 0 )  Key = 7;
          else if ( ( KeypadPortIN & 0x20 ) == 0 )  Key = 6;
          else if ( ( KeypadPortIN & 0x40 ) == 0 )  Key = 5;
          else if ( ( KeypadPortIN & 0x80 ) == 0 )  Key = 4;

          KeypadPortOUT = 0x07;
          if ( ( KeypadPortIN & 0x10 ) == 0 )  Key = 3;
          else if ( ( KeypadPortIN & 0x20 ) == 0 )  Key = 2;
          else if ( ( KeypadPortIN & 0x40 ) == 0 )  Key = 1;
          else if ( ( KeypadPortIN & 0x80 ) == 0 )  Key = 0;

          delay(10000);
          KeypadPortOUT &= ~0x0F;       // Reset Row1-4
          KeypadIRQIntPend &= ~BIT1;    // Reset Flag
      }

  //---------------------------------------------------------------------
  //            Exit from a given LPM
  //---------------------------------------------------------------------
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
//*********************************************************************
//            TimerA0 Interrupt Service Routine
//*********************************************************************
#pragma vector = TIMERA1_VECTOR
__interrupt void TimerA_ISR (void){
  if (Timer0_CTL & TAIFG){      //check if interrupt is from TimerA0
    Timer0_CTL &= ~TAIFG;            //turn off flag
    LPM0_EXIT;                      //exit LPM0
  }
}

//*********************************************************************
//            TimerB0 Interrupt Service Routine
//*********************************************************************
#pragma vector = TIMERB1_VECTOR
__interrupt void TimerB_ISR (void){
  if (Timer1_CTL & TBIFG){      //check if interrupt is from TimerA0
    Timer1_CTL &= ~TBIFG;            //turn off flag
    leds_ptr++;                     //increment pointer
    if (leds_ptr == &leds[9]){      //check if pointer is at the end of the array
        leds_ptr = &leds[0];         //reset pointer to the beginning of the array
    }

    
    LPM0_EXIT;                      //exit LPM0
  }
}

//-------------------------------------------------------------
//              StartTimer and FinishTimer
//-------------------------------------------------------------
void startTimerA(){
    if (state == state3){
        // Will interrupt after 0.5s (in the first time).
    Timer0_CCR0 = 0xFFFF;   // 65535
    Timer0_CCTL0 = CCIE; // enable interrupt
    Timer0_CTL = TASSEL_2 + MC_3 + ID_3 + TACLR; //  select: 2 - SMCLK ; control: 3 - Up/Down  ; divider: 3 - /8
    //Timer0_CTL |= TAIE; // enable interrupt

    }
    else{
    // Will interrupt after 0.5s (in the first time).
    Timer0_CCR0 = 0xFFFF;   // 65535
    Timer0_CTL = TASSEL_2 + MC_3 + ID_3 + TACLR; //  select: 2 - SMCLK ; control: 3 - Up/Down  ; divider: 3 - /8
    Timer0_CTL |= TAIE; // enable interrupt
    }
}

void startTimerB(){
    // Will interrupt after 0.5s
   Timer1_CCR0 = 0xFFFF;
    Timer1_CTL = TBSSEL_2 + MC_2 + ID_3 + TBCLR; //  select: 2 - SMCLK ; control: 1 - Up  ; divider: 3 - /8
    Timer1_CTL |= TBIE; // enable interrupt
}

void finishTimerA(){
    Timer0_CTL &= ~TAIE;
}

//*********************************************************************
//                            DMA
//*********************************************************************

void startDMA(){
    if(state==state2){      // state2 - merge
        DMA0CTL = DMADT_1 + DMASBDB + DMASRCINCR_3 + DMADSTINCR_3; // block, byte to byte, src inc, dst inc
        DMACTL0 = DMA0TSEL_0; // SW Trigger
    }
    if(state==state3){
        // DMA0CTL = DMADT_4 + DMASBDB + DMASRCINCR_3 + DMAEN; // single repeat, byte to byte, src inc, enable
        // DMACTL0 = DMA0TSEL_2; // TimerB CCR2 Trigger
        
       
        DMA0CTL = DMADT_5 + DMASBDB + DMASRCINCR_3 + DMAEN; // block repeat, byte to byte, src inc, enable
         DMACTL0 = DMA0TSEL_2; // TimerB CCR2 Trigger
    }
}

void stopDMA(){
    DMA0CTL &= ~DMAEN;
}

void triggerDMA(){
    DMA0CTL |= DMAEN + DMAREQ;  // Enable and Trigger
}
