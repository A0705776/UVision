// dac.c
// This software configures DAC output
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 3/6/17 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "SysTick.h"

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
#define PB05 (*((volatile uint32_t *) 0x400050FC))
void DAC_Init(void)
{
	//PortB init
		SYSCTL_RCGCGPIO_R |= 0x00000002; //ACTIVATE CLOCK FOR PORT B
		while((SYSCTL_PRGPIO_R&0x00000002)==0) //WAIT
		{
		}
		GPIO_PORTB_CR_R = 0x0F; //OPEN PORT B PIN 0-3
		GPIO_PORTB_DIR_R = 0x0F; //PIN 0-3 OUTPUT
		GPIO_PORTB_DEN_R = 0x0F; //DIGITAL IO
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(uint8_t data)
{
	PB05 = data;
}
