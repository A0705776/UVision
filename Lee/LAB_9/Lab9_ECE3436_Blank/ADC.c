// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 3/6/2015 
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void){ 
	SYSCTL_RCGCGPIO_R |= 0x10; // 1) activate clock for Port E
	while((SYSCTL_PRGPIO_R & 0x10) != 0x10){}; //hgfe  dcba   0001 0000  0x10
	GPIO_PORTE_DIR_R &= ~0x04; // 2) make PE4 input
	GPIO_PORTE_AFSEL_R |= 0x04; // 3) enable alternate function on PE4
	GPIO_PORTE_DEN_R &= ~0x04; // 4) disable digital I/O on PE4
	GPIO_PORTE_AMSEL_R |= 0x04;
	GPIO_PORTE_PCTL_R &=~0x04;
	SYSCTL_RCGCADC_R |= 0x01; // 6) activate ADC0
		
	for(int i = 0; i < 10; i++);
		
	ADC0_PC_R = 0x01; // 7) configure for 125K
	ADC0_SSPRI_R = 0x0123; // 8) Sequencer 3 is highest priority
	ADC0_ACTSS_R &= ~0x0008; // 9) disable sample sequencer 3
	ADC0_EMUX_R &= ~0xF000; // 10) seq3 is software trigger
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0) + 1; // set channel Ain9 (PE4) // 11) clear SS3 field
	ADC0_SSCTL3_R = 0x0006; // 12) no TS0 D0, yes IE0 END0
	ADC0_IM_R &= ~0x0008; // 13) disable SS3 interrupts
	ADC0_ACTSS_R |= 0x0008; // 14) enable sample sequencer 3
}



//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void){  
	unsigned int result;
	ADC0_PSSI_R = 0x0008; // 1) initiate SS3
	while((ADC0_RIS_R&0x08)==0){}; // 2) wait for conversion done
	result = ADC0_SSFIFO3_R&0xFFF; // 3) read 12-bit result
	ADC0_ISC_R = 0x0008; // 4) acknowledge completion
	return result;
}







