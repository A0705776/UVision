// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 3/6/2015 
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "SysTick.h"
// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void)
{ 
	SYSCTL_RCGCGPIO_R |= 0x10; //activate clock on PE
	while((SYSCTL_PRGPIO_R & 0x10) == 0) {};
	GPIO_PORTE_DIR_R &= ~0x10; //PE4 input
	GPIO_PORTE_AFSEL_R |= 0x10;
	GPIO_PORTE_DEN_R &= ~0x10;
	GPIO_PORTE_AMSEL_R |= 0x10; //analog input
	SYSCTL_RCGCADC_R |= 0x01; //ADC0
	for(int i = 0; i < 1000 ; i++){};
	ADC0_PC_R = 0x01; //125k sample/s max
	ADC0_SSPRI_R = 0x0123; //using sequence 3
	ADC0_ACTSS_R &= ~0x0008; //disable for safe practice
	ADC0_EMUX_R &=~0xF000; //software trigger
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R & 0xFFFFFFF0) + 9; //PE2 = AIN1
	ADC0_SSCTL3_R = 0x0006;
	ADC0_IM_R |= 0x0008; //disable SS3 interrupt
	ADC0_ACTSS_R |= 0x0008; //enable ADC	
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void)
{  
	uint32_t data;
	ADC0_PSSI_R = 0x0008;
	while((ADC0_RIS_R &0x08)==0){}; //wait for conversion
	data = ADC0_SSFIFO3_R & 0xFFF; //12 bit
	ADC0_ISC_R = 0x0008;
  return data;
}
