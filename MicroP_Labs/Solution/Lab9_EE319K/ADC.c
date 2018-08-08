// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 8/9/2017 
// Student names: change this to your names or look very silly <---- "never"
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "tm4c123gh6pm.h"

// PE2 initialization function for ADC
// Input: none
// Output: none
void PE_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x10; 							//setup the clock for port E
	while( (SYSCTL_PRGPIO_R&0x10)==0){};		//wait till port E actually set
	GPIO_PORTE_DIR_R &= ~0x04;							//PE2 = input							
	GPIO_PORTE_AFSEL_R |= 0x04;							//enable alternate function on PE2
	GPIO_PORTE_DEN_R &= ~0x04;							//disable digital i/o on PE2
	GPIO_PORTE_AMSEL_R |= 0x04;							//enable analog function
}

// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void){ 
	PE_Init();											//initilize the PE2 pin configuration for analog input
	SYSCTL_RCGCADC_R |= 0x1; 				//Activate ADC0
	for(int i = 0; i < 10; i++){};	//wait for ADC0 to stabilize
	ADC0_PC_R =0x01;								//configure for 125K
	ADC0_SSPRI_R = 0x0123;          //Sequencer 3 is highest priority
	ADC0_ACTSS_R &= ~0x0008;        //disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;         //seq3 is software trigger
  ADC0_SSMUX3_R &= ~0x000F;       //clear SS3 field
  ADC0_SSMUX3_R += 1;             //set channel Ain1 (PE2)
  ADC0_SSCTL3_R = 0x0006;         //set ADC modes
	ADC0_IM_R &= ~0x8;							//disable ss3 interrupts
  ADC0_ACTSS_R |= 0x0008;         //enable sample sequencer 3
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion (pg389) range 0 - 3.3v and int 0 - 4095
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

