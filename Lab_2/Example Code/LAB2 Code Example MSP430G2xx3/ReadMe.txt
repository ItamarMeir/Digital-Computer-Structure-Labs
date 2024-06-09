File Name                                         Description
----------------------------------------------------------------------------------------
msp430g2xx3_ta_01.c                               Timer_A, Toggle P1.0, CCR0 Cont. Mode ISR, DCO SMCLK
msp430g2xx3_ta_02.c                               Timer_A, Toggle P1.0, CCR0 Up Mode ISR, DCO SMCLK
msp430g2xx3_ta_03.c                               Timer_A, Toggle P1.0, Overflow ISR, DCO SMCLK
msp430g2xx3_ta_04.c                               Timer_A, Toggle P1.0, Overflow ISR, 32kHz ACLK
msp430g2xx3_ta_05.c                               Timer_A, Toggle P1.0, CCR0 Up Mode ISR, 32kHz ACLK
msp430g2xx3_ta_06.c                               Timer_A, Toggle P1.0, CCR1 Cont. Mode ISR, DCO SMCLK
msp430g2xx3_ta_07.c                               Timer_A, Toggle P1.0-2, Cont. Mode ISR, DCO SMCLK
msp430g2xx3_ta_08.c                               Timer_A, Toggle P1.0-2, Cont. Mode ISR, 32kHz ACLK
msp430g2xx3_ta_10.c                               Timer_A, Toggle P1.1/TA0, Up Mode, DCO SMCLK
msp430g2xx3_ta_11.c                               Timer_A, Toggle P1.1/TA0, Up Mode, 32kHz ACLK
msp430g2xx3_ta_13.c                               Timer_A, Toggle P1.1/TA0, Up/Down Mode, DCO SMCLK
msp430g2xx3_ta_14.c                               Timer_A, Toggle P1.1/TA0, Up/Down Mode, 32kHz ACLK
msp430g2xx3_ta_16.c                               Timer_A, PWM TA1-2, Up Mode, DCO SMCLK
msp430g2xx3_ta_17.c                               Timer_A, PWM TA1, Up Mode, 32kHz ACLK
msp430g2xx3_ta_19.c                               Timer_A, PWM TA1, Up/Down Mode, DCO SMCLK
msp430g2xx3_ta_20.c                               Timer_A, PWM TA1, Up/Down Mode, 32kHz ACLK
msp430g2xx3_ta_21.c                               Timer_A, Normal Timer Mode Input Period and Duty Cycle Measurement

msp430g2x33_adc10_01.c                            ADC10, Sample A0, Set P1.0 if A0 > 0.5*AVcc
msp430g2x33_adc10_02.c                            ADC10, Sample A1, 1.5V Ref, Set P1.0 if A1 > 0.2V
msp430g2x33_adc10_03.c                            ADC10, ADC10, Sample A10 Temp, Set P1.0 if Temp ++ ~2C
msp430g2x33_adc10_04.c                            ADC10, ADC10, Sample A1, Signed, Set P1.0 if A1 > 0.5*AVcc
msp430g2x33_adc10_05.c                            ADC10, ADC10, Sample A11, Lo_Batt, Set P1.0 if AVcc < 2.3V
msp430g2x33_adc10_06.c                            ADC10, ADC10, Output Internal Vref on P1.4 & ADCCLK on P1.3
msp430g2x33_adc10_07.c                            ADC10, DTC Sample A1 32x, AVcc, Repeat Single, DCO
msp430g2x33_adc10_08.c                            ADC10, ADC10, DTC Sample A1 32x, 1.5V, Repeat Single, DCO
msp430g2x33_adc10_09.c                            ADC10, ADC10, DTC Sample A10 32x, 1.5V, Repeat Single, DCO
msp430g2x33_adc10_10.c                            ADC10, ADC10, DTC Sample A3-01, AVcc, Single Sequence, DCO
msp430g2x33_adc10_11.c                            ADC10, ADC10, Sample A1, 1.5V, TA1 Trig, Set P1.0 if > 0.5V
msp430g2x33_adc10_12.c                            ADC10, Sample A7, 1.5V, TA1 Trig, Ultra-Low Pwr
msp430g2x33_adc10_13.c                            ADC10, DTC Sample A1 32x, AVcc, TA0 Trig, DCO
msp430g2x33_adc10_14.c                            ADC10, DTC Sample A1-0 16x, AVcc, Repeat Seq, DCO
msp430g2x33_adc10_16.c                            ADC10, ADC10, DTC Sample A0 -> TA1, AVcc, DCO
msp430g2x33_adc10_temp.c                          ADC10, Sample A10 Temp and Convert to oC and oF