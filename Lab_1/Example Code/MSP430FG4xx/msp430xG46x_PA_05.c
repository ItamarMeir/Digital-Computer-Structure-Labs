//******************************************************************************
//   MSP430xG46x Demo - Write a Word to Port A (Port7+Port8)
//
//   Description: Writes a Word(FFFFh) to Port A and stays in LPM4
//   ACLK = 32.768kHz, MCLK = SMCLK = default DCO
//******************************************************************************
#include  <msp430xG46x.h>

void main(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT

  PADIR |= 0x0FFFF;                         // PA.x output
  PAOUT |= 0x0FFFF;                         // Set all PA pins HI

  _BIS_SR(LPM4_bits + GIE);                 // LPM4
}
