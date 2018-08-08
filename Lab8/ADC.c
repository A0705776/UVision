// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0 on PE2
// Last Modified: 3/6/2015 
// Student names: Jerry Yang/Aditya Tyagi

#include <stdint.h>
#include "ADC.h"
#include "tm4c123gh6pm.h"
void PE_Init(void);

// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void){ volatile unsigned long delay = 0;
	PE_Init();
	SYSCTL_RCGCADC_R |= 0x1; 	//Activate ADC0, SYSCTL_RCGCADC_R
	delay++; delay++; 	delay++; delay++;delay++; delay++;						//Delay to stabilize
	ADC0_PC_R =0x01;								//configure for 125K, SYSCTL_RCGCADC_R
	ADC0_ACTSS_R &= ~0x0008;        //disable sample sequencer 3
//	ADC0_SAC_R = 6;									//Slow down fluctuations
  ADC0_SSPRI_R = 0x0123;          //Sequencer 3 is highest priority
  ADC0_EMUX_R &= ~0xF000;         //seq3 is software trigger
  ADC0_SSMUX3_R &= ~0x000F;       //clear SS3 field
  ADC0_SSMUX3_R += 1;             //set channel Ain1 (PE2)
  ADC0_SSCTL3_R = 0x0006;         //set ADC modes
	ADC0_IM_R &= ~0x8;
  ADC0_ACTSS_R |= 0x0008;         //enable sample sequencer 3
	
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void){  uint32_t result;
  ADC0_PSSI_R = 0x08;           		// 1) initiate SS3
	result++; result--;result++; result--;
  while((ADC0_RIS_R&0x08)==0){};   // 2) wait for conversion done
  result = ADC0_SSFIFO3_R&0xFFF;   // 3) read result
  ADC0_ISC_R = 0x0008;             // 4) acknowledge completion
  return result;
}


// PE2 initialization function for ADC
// Input: none
// Output: none
void PE_Init(void){
		SYSCTL_RCGCGPIO_R |= 0x10;
	while( (SYSCTL_PRGPIO_R&0x10)==0){};
	GPIO_PORTE_DIR_R &= ~0x4;
	GPIO_PORTE_AFSEL_R |= 0x04;
	GPIO_PORTE_DEN_R &= ~0x04;
	GPIO_PORTE_AMSEL_R |= 0x04;
}
